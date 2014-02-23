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