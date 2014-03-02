// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef PLAYER_H_
#define PLAYER_H_

#include "library.h"
#include "audio_manager.h"

class MainWindow;

class Player {
 public:
  Player();
  ~Player();

  void Init(Library* library, AudioManager* audio_manager,
            MainWindow* main_window);

  void Play(int index);

  Library* GetLibrary() const { return library_; }

 private:
  bool playing_;

  Library* library_;
  AudioManager* audio_manager_;
  MainWindow* main_window_;
};

#endif  // PLAYER_H_
