// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "timer.h"

#include <QtCore/QDebug>

Timer::Timer(int sec, int msec, bool is_repeating, std::function<void()> callback)
: sec_(sec), msec_(msec), is_repeating_(is_repeating), callback_(callback) {
}

Timer::~Timer() {
}

void Timer::Start() {

}

void Timer::Stop() {

}
