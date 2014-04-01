// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "gst_player.h"

#include "g_own_ptr.h"
#include "base/bind.h"
#include "base/logging.h"
#include "base/file_util.h"
#include "base/files/file_path.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"

#include <iostream>
#include <cmath>

// static
GstPlayer* GstPlayer::instance_ = NULL;

GstPlayer::GstPlayer() 
    : playing_(false),
      seeking_(false),
      position_(0.0f),
      duration_(0.0f) {
  GOwnPtr<GError> error;  
  if (!gst_init_check(NULL, NULL, &error.outPtr())) {
    LOG(ERROR) << "Gstreamer init failed: " << error->message;
  }
  LOG(INFO) << "Gstreamer: " << gst_version_string();

  playbin_ = gst_element_factory_make("playbin", "playbin");

  GstBus* bus_ = gst_pipeline_get_bus(GST_PIPELINE(playbin_));
  gst_bus_add_signal_watch(bus_);
  g_signal_connect(bus_, "message", G_CALLBACK(__OnBusMessage), this);
  gst_object_unref(bus_);

  gst_debug_set_active(TRUE);
  gst_debug_set_default_threshold(GST_LEVEL_WARNING);
  //gst_debug_set_default_threshold(GST_LEVEL_DEBUG);

  gst_debug_remove_log_function(gst_debug_log_default);
  gst_debug_add_log_function(__OnLogMessage, this, NULL);

  LOG(INFO) << "GstPlayer()";
}

GstPlayer::~GstPlayer() {
  if (playbin_) {
    gst_element_set_state(playbin_, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(playbin_));
    playbin_ = 0;
  }

  gst_deinit();
  LOG(INFO) << "~GstPlayer()";
}

// static
void GstPlayer::CreateInstance() {
  if (!instance_)
    instance_ = new GstPlayer;
}

// static
GstPlayer* GstPlayer::GetInstance() {
  DCHECK(instance_) << "GstPlayer::CreateInstance must be called before getting "
      "the instance of GstPlayer.";
  return instance_;
}

// static
void GstPlayer::DeleteInstance() {
  delete instance_;
  instance_ = NULL;
}

void GstPlayer::AddObserver(GstPlayerObserver* observer) {
    observers_.AddObserver(observer);
}
void GstPlayer::RemoveObserver(GstPlayerObserver* observer) {
    observers_.RemoveObserver(observer);
}


// static
void GstPlayer::__OnLogMessage(
    GstDebugCategory* category, GstDebugLevel level,
    const gchar* file, const gchar* function, gint line,
    GObject* object, GstDebugMessage* message, gpointer user_data) {
  return static_cast<GstPlayer*>(user_data)->OnLogMessage(
      category, level, file, function, line, object, message);
}

void GstPlayer::OnLogMessage(
    GstDebugCategory* category, GstDebugLevel level,
    const gchar* file, const gchar* function, gint line,
    GObject* object, GstDebugMessage* message) {
  if (level > gst_debug_category_get_threshold(category))
    return;

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

    LOG(INFO) << gst_debug_category_get_name(category) << " " << file << "("
              << line << ") " << function << ": " << obj << " "
              << gst_debug_message_get(message);
  } else {
    LOG(INFO) << gst_debug_category_get_name(category) << " " << file << "("
              << line << ") " << function << ": "
              << gst_debug_message_get(message);
  }
}

void GstPlayer::Load(const std::string& uri) {
  if (playing_)
    _Stop();
  else {
    // FIXME(brbrooks) keep track of both pause and playing state instead
    // of doing this here.
    gst_element_set_state(playbin_, GST_STATE_READY);
  }

  // FIXME(brbrooks) fix this
  if (!StartsWithASCII(uri, "http://", false)) {
    std::string loc = gst_filename_to_uri(uri.c_str(), NULL);
    g_object_set(G_OBJECT(playbin_), "uri", loc.c_str(), NULL);
  } else {
    g_object_set(G_OBJECT(playbin_), "uri", uri.c_str(), NULL);
  }
  LOG(INFO) << "load " << uri;

  gst_element_set_state(playbin_, GST_STATE_PAUSED);
}

void GstPlayer::_Play() {
  // FIXME(brbrooks) DCHECK(loaded_); so that we can't play/pause an invalid uri
  DCHECK(!playing_);
  gst_element_set_state(playbin_, GST_STATE_PLAYING);
  playing_ = true;
  StartTrackPoller();
}
void GstPlayer::Play() {
  if (!playing_)
    _Play();
}
void GstPlayer::_Pause() {
  DCHECK(playing_);
  gst_element_set_state(playbin_, GST_STATE_PAUSED);
  playing_ = false;
  StopTrackPoller();
}
void GstPlayer::Pause() {
  if (playing_)
    _Pause();
}
void GstPlayer::_Stop() {
  DCHECK(playing_);
  gst_element_set_state(playbin_, GST_STATE_READY);
  playing_ = false;
  StopTrackPoller();
}

void GstPlayer::Seek(float time) {
  if (seeking_)
    return;

  if (time == GetPosition())
    return;
  LOG(INFO) << "seek request";
  double s;
  float ms = modf(time, &s) * 1000000;
  GTimeVal tv;
  tv.tv_sec  = static_cast<glong>(s);
  tv.tv_usec = static_cast<glong>(roundf(ms / 10000) * 10000);

  GstClockTime clock_time = GST_TIMEVAL_TO_TIME(tv);
  gdouble rate = 1.0;
  seeking_ = true;
  if (!gst_element_seek(playbin_, rate,
                        GST_FORMAT_TIME,
                        (GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                        GST_SEEK_TYPE_SET, clock_time,
                        GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
    LOG(ERROR) << "Seek failed.";
    seeking_ = false;
  }
}

float GstPlayer::GetPosition() const {
  return position_;
}
float GstPlayer::GetDuration() const {
  return duration_;
}

void GstPlayer::QueryPosition() {
  gint64 pos;

  if (!gst_element_query_position(playbin_, GST_FORMAT_TIME, &pos))
    LOG(ERROR) << "Query position failed.";

  if (pos != static_cast<gint64>(GST_CLOCK_TIME_NONE))
    position_ = static_cast<float>(pos) / static_cast<float>(GST_SECOND);
  else
    position_ = 0.0f;

  NotifyPositionUpdated(position_);
}

void GstPlayer::QueryDuration() {
  gint64 dur;
  float duration;

  if (!gst_element_query_duration(playbin_, GST_FORMAT_TIME, &dur))
    LOG(ERROR) << "Query duration failed.";

  duration = static_cast<float>(dur) / static_cast<float>(GST_SECOND);

  if (duration != duration_) {
    if (dur != static_cast<gint64>(GST_CLOCK_TIME_NONE))
      duration_ = duration;
    else
      duration_ = 0.0f;

    NotifyDurationUpdated(duration_);
  }
}

// static
gboolean GstPlayer::__OnBusMessage(GstBus* bus, GstMessage* msg,
                                      gpointer data) {
  return static_cast<GstPlayer*>(data)->OnBusMessage(bus, msg);
}

gboolean GstPlayer::OnBusMessage(GstBus* bus, GstMessage* msg) {
  GOwnPtr<gchar> debug;
  GOwnPtr<GError> error;
  
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
      _Stop();
      NotifyEndOfStream();
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
      
      break;
    case GST_MESSAGE_DURATION_CHANGED:
      // TODO(brbrooks) it seems like it takes at least 2-5 duration queries
      // to get the real & constant duration for current track.  But, this
      // message is only sent to the bus for the first change in duration,
      // which doesn't reflect the real duration.  QueryDuration() in track
      // poller, but only update if there is a difference.  Figure out why
      // this message is only sent once, and why it takes a few queries to
      // get the real duration.
      QueryDuration();
      break;
    case GST_MESSAGE_ASYNC_DONE:
      if (seeking_) {
        LOG(INFO) << "done seeking";
        seeking_ = false;
      }
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

void GstPlayer::TrackPollerWork() {
  if (!seeking_) {
    QueryPosition();
    QueryDuration();
  }
}
void GstPlayer::StartTrackPoller() {
  track_poller_.Start(FROM_HERE, base::TimeDelta::FromMilliseconds(200),
                      this, &GstPlayer::TrackPollerWork);
}
void GstPlayer::StopTrackPoller() {
  track_poller_.Stop();
}

void GstPlayer::NotifyEndOfStream() {
  FOR_EACH_OBSERVER(GstPlayerObserver, observers_, OnEndOfStream());
}
void GstPlayer::NotifyPositionUpdated(float position) {
  FOR_EACH_OBSERVER(GstPlayerObserver, observers_, OnPositionUpdated(position));
}
void GstPlayer::NotifyDurationUpdated(float duration) {
  FOR_EACH_OBSERVER(GstPlayerObserver, observers_, OnDurationUpdated(duration));
}
