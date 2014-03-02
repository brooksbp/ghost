// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "player.h"

Player::Player() {
}

Player::~Player() {
}

void Player::Init(Library* library, AudioManager* audio_manager,
                  MainWindow* main_window) {
  library_ = library;
  audio_manager_ = audio_manager;
  main_window_ = main_window;
}

void Player::Play(int index) {
  Track* track = library_->GetTrack(index);
  if (track) {
    audio_manager_->PlayMp3File(track->file_path_);
    playing_ = true;
  }
}

void Player::Pause() {
  if (playing_) {
    audio_manager_->Pause();
    playing_ = false;
  }
}

void Player::Resume() {
  if (!playing_) {
    audio_manager_->Resume();
    playing_ = true;
  }
}
