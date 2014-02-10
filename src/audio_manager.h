#ifndef AUDIO_MANAGER_H_
#define AUDIO_MANAGER_H_

#include <functional>

#include <gst/gst.h>
#include <glib.h>

// Abstract all of Gstreamer and audio control here..

class AudioManager {
 public:
  AudioManager();
  ~AudioManager();

  void PlayMp3File(const char* file);
  void TogglePause(void);

  std::function<void()> eosCallback;
  
 private:
  static gboolean GstBusCallback(GstBus* bus, GstMessage* msg, gpointer data);

  GstElement* pipeline_;

  GstBus* bus_;
  
  GstElement* source_;
  GstElement* decoder_;
  GstElement* sink_;

  int paused_;
};

#endif  // AUDIO_MANAGER_H_
