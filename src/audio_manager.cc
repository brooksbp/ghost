// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "audio_manager.h"

#include "g_own_ptr.h"
#include "base/logging.h"
#include "base/file_util.h"
#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"

#include <iostream>


static void gst_debug_logcat(GstDebugCategory* category,
  GstDebugLevel level,
  const gchar* file,
  const gchar* function,
  gint line,
  GObject* object,
  GstDebugMessage* message,
  gpointer unused) {
  GOwnPtr<gchar> tag;

  if (level > gst_debug_category_get_threshold(category))
    return;

  tag.outPtr() = g_strdup_printf("GStreamer+%s", gst_debug_category_get_name(category));

  if (object) {
    GOwnPtr<gchar> obj;

    if (GST_IS_PAD(object) && GST_OBJECT_NAME(object)) {
      obj.outPtr() = g_strdup_printf("<%s:%s>", GST_DEBUG_PAD_NAME(object));
    } else if (GST_IS_OBJECT(object) && GST_OBJECT_NAME(object)) {
      obj.outPtr() = g_strdup_printf("<%s>", GST_OBJECT_NAME(object));
    } else if (G_IS_OBJECT(object)) {
      obj.outPtr() = g_strdup_printf("<%s@%p>", G_OBJECT_TYPE_NAME(object), object);
    } else {
      obj.outPtr() = g_strdup_printf("<%p>", object);
    }

    LOG(INFO) << file << ":" << line << ":" << function
              << " " << obj << " " << gst_debug_message_get(message);
  } else {
    LOG(INFO) << file << ":" << line << ":" << function
              << " " << gst_debug_message_get(message);
  }
}

AudioManager::AudioManager() {
  gst_init(NULL, NULL);

  playbin_ = gst_element_factory_make("playbin", "playbin");

  GstBus* bus_ = gst_pipeline_get_bus(GST_PIPELINE(playbin_));
  gst_bus_add_signal_watch(bus_);
  g_signal_connect(bus_, "message", G_CALLBACK(__OnBusMessage), this);
  gst_object_unref(bus_);

  gst_debug_set_active(TRUE);
  gst_debug_set_default_threshold(GST_LEVEL_WARNING);
  gst_debug_remove_log_function(gst_debug_log_default);
  gst_debug_add_log_function((GstLogFunction)gst_debug_logcat, NULL, NULL);

  track_poller_ = new Timer(0, 100, true,
                            std::bind(&AudioManager::TrackPoller, this));
}

AudioManager::~AudioManager() {
  if (playbin_) {
    gst_element_set_state(playbin_, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(playbin_));
    playbin_ = 0;
  }

  gst_deinit();
}

void AudioManager::PlayURI(std::string& uri) {
  gst_element_set_state(playbin_, GST_STATE_READY);

  g_object_set(G_OBJECT(playbin_), "uri", uri.c_str(), NULL);

  gst_element_set_state(playbin_, GST_STATE_PLAYING);

  playing_ = true;
}

void AudioManager::PlayMp3File(base::FilePath& file) {
#if defined(OS_WIN)
  std::string loc = base::WideToUTF8(file.value());
#else
  std::string loc = file.value();
#endif
  std::string loc2 = gst_filename_to_uri(loc.c_str(), NULL);
  PlayURI(loc2);
  track_poller_->Start();
}

// TODO(brbrooks) playing/pause/resume/etc logic needs to be abstracted into
// single functions so that different entry points can trigger them.
void AudioManager::Pause() {
  gst_element_set_state(playbin_, GST_STATE_PAUSED);
}
void AudioManager::Resume() {
  gst_element_set_state(playbin_, GST_STATE_PLAYING);
}

void AudioManager::TrackPoller() {
  if (playing_) {
#if defined(OS_POSIX)
    gst_element_query_position(playbin_, GST_FORMAT_TIME, &pos_);
    gst_element_query_duration(playbin_, GST_FORMAT_TIME, &len_);
#elif defined(OS_WIN)
    // gstreamer-0.10 compat?
    GstFormat fmt = GST_FORMAT_TIME;
    gst_element_query_position(playbin_, &fmt, &pos_);
    gst_element_query_duration(playbin_, &fmt, &len_);
#endif
    PlaybackProgressCallback(pos_, len_);
  } else {
    track_poller_->Stop();
  }
}

// static
gboolean AudioManager::__OnBusMessage(GstBus* bus, GstMessage* msg,
                                      gpointer data) {
  return static_cast<AudioManager*>(data)->OnBusMessage(bus, msg);
}

gboolean AudioManager::OnBusMessage(GstBus* bus, GstMessage* msg) {
  GOwnPtr<gchar> debug;
  GOwnPtr<GError> error;
  
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
      // TODO(brbrooks) Move this into own fn
      playing_ = false;
      track_poller_->Stop();
      eosCallback();
      LOG(INFO) << "End of stream.";
      break;
    case GST_MESSAGE_STATE_CHANGED:
      if (GST_MESSAGE_SRC(msg) == reinterpret_cast<GstObject*>(playbin_)) {
        GstState old_state, new_state;
        gst_message_parse_state_changed(msg, &old_state, &new_state, NULL);
        LOG(INFO) << GST_MESSAGE_SRC(msg) << " State " << old_state << " -> " << new_state;
      }
      break;
    case GST_MESSAGE_STREAM_STATUS:
      GstStreamStatusType type;
      GstElement* owner;
      gst_message_parse_stream_status(msg, &type, &owner);
      LOG(INFO) << owner << " stream status " << type;
      break;
    case GST_MESSAGE_STREAM_START:
      // "Useful e.g. when using playbin in gapless playback mode, to get
      // notified when the next title actually starts playing (which will
      // be some time after the URI for the next title has been set).
      
      // Ignore for now.
      break;
    case GST_MESSAGE_DURATION_CHANGED:
      // Can now get the duration with a query.

      // Ignore for now.
      break;
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(msg, &error.outPtr(), &debug.outPtr());
      LOG(ERROR) << "Error: " << error->code << " " << error->message;
      break;
    case GST_MESSAGE_UNKNOWN:
    case GST_MESSAGE_WARNING:
    case GST_MESSAGE_INFO:
    case GST_MESSAGE_BUFFERING:
    case GST_MESSAGE_STATE_DIRTY:
    case GST_MESSAGE_STEP_DONE:
    case GST_MESSAGE_TAG:
    case GST_MESSAGE_CLOCK_PROVIDE:
    case GST_MESSAGE_CLOCK_LOST:
    case GST_MESSAGE_NEW_CLOCK:
    case GST_MESSAGE_STRUCTURE_CHANGE:
    case GST_MESSAGE_APPLICATION:
    case GST_MESSAGE_ELEMENT:
    case GST_MESSAGE_SEGMENT_START:
    case GST_MESSAGE_SEGMENT_DONE:
    case GST_MESSAGE_LATENCY:
    case GST_MESSAGE_ASYNC_START:
    case GST_MESSAGE_ASYNC_DONE:
    case GST_MESSAGE_REQUEST_STATE:
    case GST_MESSAGE_STEP_START:
    case GST_MESSAGE_QOS:
    case GST_MESSAGE_PROGRESS:
    case GST_MESSAGE_TOC:
    case GST_MESSAGE_RESET_TIME:
    case GST_MESSAGE_NEED_CONTEXT:
    case GST_MESSAGE_HAVE_CONTEXT:
    case GST_MESSAGE_ANY:
    default:
      LOG(WARNING) << "Unhandled msg: " << GST_MESSAGE_TYPE_NAME(msg);
      break;
  }
  
  return TRUE;
}
