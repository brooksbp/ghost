// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef AUDIO_MANAGER_H_
#define AUDIO_MANAGER_H_

#include <functional>

#include <gst/gst.h>
#include <glib.h>
#include <string>

#include "timer.h"
#include "base/basictypes.h"
#include "base/files/file_path.h"

// Gstreamer stuff..
class AudioManager {
 public:
  AudioManager();
  ~AudioManager();

  void PlayURI(std::string& uri);
  void PlayMp3File(base::FilePath& file);

  void Pause();
  void Resume();

  std::function<void()> eosCallback;
  std::function<void(int64_t&, int64_t&)> PlaybackProgressCallback;
  
 private:

  static gboolean __OnBusMessage(GstBus* bus, GstMessage* msg, gpointer data);
  gboolean OnBusMessage(GstBus* bus, GstMessage* msg);

  GstElement* playbin_;

  bool playing_;

  // Stream position in nanoseconds.
  int64_t pos_;
  // Stream duration in nanoseconds.
  int64_t len_;
  
  Timer* track_poller_;
  void TrackPoller();
};

#endif  // AUDIO_MANAGER_H_
