// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "timer.h"

#include <QtCore/QDebug>

Timer::Timer(int sec, int msec, bool is_repeating, std::function<void()> callback)
    : sec_(sec),
      msec_(msec),
      is_repeating_(is_repeating),
      callback_(callback) {
}

Timer::~Timer() {
}

void Timer::Start() {
  struct sigaction sa;
  struct sigevent sev;

  // Establish handler for notification signal.
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = Timer::handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGRTMAX, &sa, NULL);

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMAX;
  sev.sigev_value.sival_ptr = this;
  timer_create(CLOCK_REALTIME, &sev, &timerid_);

  its_.it_value.tv_sec = sec_;
  its_.it_value.tv_nsec = 1000 * 1000 * msec_;
  its_.it_interval.tv_sec = (is_repeating_) ? sec_ : 0;
  its_.it_interval.tv_nsec = (is_repeating_) ? 1000 * 1000 * msec_ : 0;

  timer_settime(timerid_, 0, &its_, NULL);
}

void Timer::Stop() {
  its_.it_value.tv_sec = 0;
  its_.it_value.tv_nsec = 0;
  its_.it_interval.tv_sec = 0;
  its_.it_interval.tv_nsec = 0;

  timer_settime(timerid_, 0, &its_, NULL);
}

void Timer::handler(int sig, siginfo_t* si, void* uc) {
  Timer* this_ = static_cast<Timer*>(si->si_value.sival_ptr);
  this_->callback_();
}
