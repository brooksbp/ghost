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
#include "base/observer_list.h"
#include "base/timer/timer.h"

class GstPlayer {
 public:
  GstPlayer();
  ~GstPlayer();

  static void CreateInstance();
  static GstPlayer* GetInstance();
  static void DeleteInstance();

  class GstPlayerObserver {
   public:
    virtual void OnEndOfStream() = 0;
    virtual void OnPositionUpdated(float position) = 0;
    virtual void OnDurationUpdated(float duration) = 0;
  };

  void AddObserver(GstPlayerObserver* observer);
  void RemoveObserver(GstPlayerObserver* observer);

  void Load(const std::string& uri);

  void Play();
  void Pause();

  void Seek(float time);

  // Get the current playback position in seconds.
  float GetPosition() const;
  // Get the current stream's duration in seconds.
  float GetDuration() const;
  
 private:
  void QueryPosition();
  void QueryDuration();

  void NotifyEndOfStream();
  void NotifyPositionUpdated(float position);
  void NotifyDurationUpdated(float duration);

  bool playing_;
  bool seeking_;

  float position_;
  float duration_;

  GstElement* playbin_;

  ObserverList<GstPlayerObserver> observers_;

  static GstPlayer* instance_;

  static gboolean __OnBusMessage(GstBus* bus, GstMessage* msg, gpointer data);
  gboolean OnBusMessage(GstBus* bus, GstMessage* msg);

  static void __OnLogMessage(
      GstDebugCategory* category, GstDebugLevel level,
      const gchar* file, const gchar* function, gint line,
      GObject* object, GstDebugMessage* message, gpointer user_data);
  void OnLogMessage(
      GstDebugCategory* category, GstDebugLevel level,
      const gchar* file, const gchar* function, gint line,
      GObject* object, GstDebugMessage* message);


  // Timer for a poller that does work such as queries playback progress while
  // a tracking is playing.
  base::RepeatingTimer<GstPlayer> track_poller_;
  
  void StartTrackPoller();
  void StopTrackPoller();
  void TrackPollerWork();

  DISALLOW_COPY_AND_ASSIGN(GstPlayer);
};

#endif  // GST_PLAYER_H_
