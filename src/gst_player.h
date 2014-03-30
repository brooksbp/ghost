// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef GST_PLAYER_H_
#define GST_PLAYER_H_

#include <gst/gst.h>
#include <glib.h>
#include <string>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/timer/timer.h"

class GstPlayer : public base::RefCountedThreadSafe<GstPlayer> {
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

  base::Callback<void()> OnEndOfStream;
  base::Callback<void(float&)> OnPositionUpdated;
  base::Callback<void(float&)> OnDurationUpdated;
  
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
  base::RepeatingTimer<GstPlayer> track_poller_;
  void TrackPoller();
  void StartTrackPoller() {
    track_poller_.Start(FROM_HERE, base::TimeDelta::FromMilliseconds(200),
                        this, &GstPlayer::TrackPoller);
  }
  void StopTrackPoller() {
    track_poller_.Stop();
  }
};

#endif  // GST_PLAYER_H_
