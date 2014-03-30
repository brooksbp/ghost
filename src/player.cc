// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "player.h"

#include "base/strings/utf_string_conversions.h"

Player::Player() {
  LOG(INFO) << "Player()";
}

Player::~Player() {
  LOG(INFO) << "~Player()";
}

void Player::Init(MainWindow* main_window) {
  main_window_ = main_window;
}

void Player::Play(int index) {
  Track* track = Library::GetInstance()->GetTrack(index);
  if (track) {
#if defined(OS_WIN)
    GstPlayer::GetInstance()->Load(base::WideToUTF8(track->file_path_.value()));
#elif defined(OS_POSIX)
    GstPlayer::GetInstance()->Load(track->file_path_.value());
#endif
    GstPlayer::GetInstance()->Play();
    playing_ = true;
  }
}

void Player::Pause() {
  if (playing_) {
    GstPlayer::GetInstance()->Pause();
    playing_ = false;
  }
}

void Player::Resume() {
  if (!playing_) {
    GstPlayer::GetInstance()->Play();
    playing_ = true;
  }
}

void Player::Seek(float time) {
  GstPlayer::GetInstance()->Seek(time);
}
