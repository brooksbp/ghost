// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef GST_PLAYER_H_
#define GST_PLAYER_H_

#include <functional>

#include <gst/gst.h>
#include <glib.h>
#include <string>

#include "timer.h"
#include "base/basictypes.h"
#include "base/files/file_path.h"

class GstPlayer {
 public:
  GstPlayer();
  ~GstPlayer();

  void Load(const std::string& uri);

  void Play();
  void Pause();

  void Seek(float time);

  // Get the current playback position in seconds.
  float GetPosition() const;
  // Get the current stream's duration in seconds.
  float GetDuration() const;

  std::function<void()> OnEndOfStream;
  std::function<void(float&)> OnPositionUpdated;
  std::function<void(float&)> OnDurationUpdated;
  
 private:

  bool playing_;

  bool seeking_;

  void QueryPosition();
  void QueryDuration();

  // Cached copy of current track's position.
  float position_;
  // Cached copy of current tracks' duration.
  float duration_;

  GstElement* playbin_;

  static gboolean __OnBusMessage(GstBus* bus, GstMessage* msg, gpointer data);
  gboolean OnBusMessage(GstBus* bus, GstMessage* msg);

  // Timer for a poller that does work such as queries playback progress while
  // a tracking is playing.
  Timer* track_poller_;
  void TrackPoller();
};

#endif  // GST_PLAYER_H_
