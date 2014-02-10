#include "timer.h"

#include <QtCore/QDebug>

Timer::Timer(int sec, int nsec, bool is_repeating, std::function<void()> callback)
    : sec_(sec),
      nsec_(nsec),
      is_repeating_(is_repeating),
      callback_(callback) {
}

void Timer::Start() {
  struct itimerspec its;
  struct sigaction sa;
  struct sigevent sev;
  timer_t timerid;

  // Establish handler for notification signal.
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = Timer::handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGRTMAX, &sa, NULL);

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMAX;
  sev.sigev_value.sival_ptr = this;
  timer_create(CLOCK_REALTIME, &sev, &timerid);

  its.it_value.tv_sec = sec_;
  its.it_value.tv_nsec = nsec_;
  its.it_interval.tv_sec = (is_repeating_) ? its.it_value.tv_sec : 0;
  its.it_interval.tv_nsec = (is_repeating_) ? its.it_value.tv_nsec : 0;

  timer_settime(timerid, 0, &its, NULL);
}

void Timer::Stop() {
}

Timer::~Timer() {
}

void Timer::handler(int sig, siginfo_t* si, void* uc) {
  Timer* this_ = static_cast<Timer*>(si->si_value.sival_ptr);
  qDebug() << "popped";
  this_->callback_();
}
