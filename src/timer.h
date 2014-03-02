// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef TIMER_H_
#define TIMER_H_

#include "base/build_config.h"

#include <functional>

#if defined(OS_POSIX)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309
#endif
#include <signal.h>
#include <time.h>
#endif

class Timer {  
 public:
  Timer(int sec, int msec, bool is_repeating, std::function<void()> callback);
  ~Timer();

  void Start();
  void Stop();

private:
  int sec_;
  int msec_;
  bool is_repeating_; 
  std::function<void()> callback_;

#if defined(OS_POSIX)        
  static void handler(int sig, siginfo_t* si, void* uc);
  timer_t timerid_;
  struct itimerspec its_;
#endif
};

#endif  // TIMER_H_
