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

  std::function<void()> eosCallback;
  std::function<void(int64_t&, int64_t&)> PlaybackProgressCallback;
  
 private:

  static gboolean __OnBusMessage(GstBus* bus, GstMessage* msg, gpointer data);
  gboolean OnBusMessage(GstBus* bus, GstMessage* msg);

  GstElement* playbin_;

  bool playing_;
};

#endif  // GST_PLAYER_H_
