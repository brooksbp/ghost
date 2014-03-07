// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef PLAYER_H_
#define PLAYER_H_

#include "library.h"
#include "gst_player.h"

class MainWindow;

class Player {
 public:
  Player();
  ~Player();

  void Init(Library* library, GstPlayer* gst_player,
            MainWindow* main_window);

  // Play the Track located at |index| in the library.
  void Play(int index);

  // Pause the current Track, if playing.
  void Pause();

  // Resume the current Track, if paused.
  void Resume();

  void Seek(float time);

  // Returns true if a file or stream is currently playing.
  bool IsPlaying() const { return playing_; }

  Library* GetLibrary() const { return library_; }

 private:
  bool playing_;

  Library* library_;
  GstPlayer* gst_player_;
  MainWindow* main_window_;
};

#endif  // PLAYER_H_
