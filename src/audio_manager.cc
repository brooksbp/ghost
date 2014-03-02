// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "audio_manager.h"

#include "base/strings/utf_string_conversions.h"

#include "base/file_util.h"
#include "base/files/file_path.h"

#include <QtCore/QDebug>
#include <iostream>

static void gst_debug_logcat(GstDebugCategory* category,
  GstDebugLevel level,
  const gchar* file,
  const gchar* function,
  gint line,
  GObject* object,
  GstDebugMessage* message,
  gpointer unused) {
  gchar* tag;

  if (level > gst_debug_category_get_threshold(category))
    return;

  tag = g_strdup_printf("GStreamer+%s", gst_debug_category_get_name(category));

  if (object) {
    gchar* obj;

    if (GST_IS_PAD(object) && GST_OBJECT_NAME(object)) {
      obj = g_strdup_printf("<%s:%s>", GST_DEBUG_PAD_NAME(object));
    } else if (GST_IS_OBJECT(object) && GST_OBJECT_NAME(object)) {
      obj = g_strdup_printf("<%s>", GST_OBJECT_NAME(object));
    } else if (G_IS_OBJECT(object)) {
      obj = g_strdup_printf("<%s@%p>", G_OBJECT_TYPE_NAME(object), object);
    } else {
      obj = g_strdup_printf("<%p>", object);
    }

    qDebug() << file << ":" << line << ":" << function << " " << obj << " " << gst_debug_message_get(message);

    g_free(obj);
  } else {
    qDebug() << file << ":" << line << ":" << function << " " << gst_debug_message_get(message);
  }

  g_free(tag);
}

AudioManager::AudioManager() {
  gst_init(NULL, NULL);

  playbin_ = gst_element_factory_make("playbin", "playbin");

  bus_ = gst_pipeline_get_bus(GST_PIPELINE(playbin_));

  gst_bus_add_signal_watch(bus_);
  g_signal_connect(bus_, "message", G_CALLBACK(AudioManager::GstBusCallback), this);

  gst_debug_set_active(TRUE);
  gst_debug_set_default_threshold(GST_LEVEL_WARNING);
  gst_debug_remove_log_function(gst_debug_log_default);
  gst_debug_add_log_function((GstLogFunction)gst_debug_logcat, NULL, NULL);

  track_poller_ = new Timer(0, 100, true,
                            std::bind(&AudioManager::TrackPoller, this));
}

AudioManager::~AudioManager() {
  gst_element_set_state(pipeline_, GST_STATE_NULL);
  gst_object_unref(bus_);
  gst_object_unref(GST_OBJECT(pipeline_));
}

void AudioManager::PlayURI(std::string& uri) {
  gst_element_set_state(playbin_, GST_STATE_READY);

  g_object_set(G_OBJECT(playbin_), "uri", uri.c_str(), NULL);

  gst_element_set_state(playbin_, GST_STATE_PLAYING);

  playing_ = 1;
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

gboolean AudioManager::GstBusCallback(GstBus* bus, GstMessage* msg,
                                      gpointer data) {
  AudioManager* this_ = static_cast<AudioManager*>(data);
  
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
      this_->playing_ = 0;
      this_->track_poller_->Stop();

      this_->eosCallback();
      break;
    case GST_MESSAGE_ERROR: {
      gchar* debug;
      GError* error;
      gst_message_parse_error(msg, &error, &debug);
      g_free(debug);
      g_printerr("Error: %s\n", error->message);
      g_error_free(error);
      break;
    }
    default:
      break;
  }
  
  return TRUE;
}
