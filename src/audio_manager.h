#ifndef AUDIO_MANAGER_H_
#define AUDIO_MANAGER_H_

#include <functional>

#include <gst/gst.h>
#include <glib.h>

#include "timer.h"
#include "base/basictypes.h"
#include "base/file_path.h"

// Abstract all of Gstreamer and audio control here..

class AudioManager {
 public:
  AudioManager();
  ~AudioManager();

  void PlayMp3File(base::FilePath& file);

  std::function<void()> eosCallback;
  std::function<void(int64_t&, int64_t&)> PlaybackProgressCallback;
  
 private:
  static gboolean GstBusCallback(GstBus* bus, GstMessage* msg, gpointer data);

  GstElement* pipeline_;

  GstBus* bus_;
  
  GstElement* source_;
  GstElement* decoder_;
  GstElement* sink_;

  int playing_;

  // Stream position in nanoseconds.
  int64_t pos_;
  // Stream duration in nanoseconds.
  int64_t len_;
  
  Timer* track_poller_;
  void TrackPoller();
};

#endif  // AUDIO_MANAGER_H_
