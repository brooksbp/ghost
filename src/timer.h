#ifndef TIMER_H_
#define TIMER_H_

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309
#endif
#include <signal.h>
#include <time.h>

#include <functional>

// Middle-ware for POSIX.1b interval timers.
class Timer {
 public:
  Timer(int sec, int nsec, bool is_repeating, std::function<void()> callback);
  ~Timer();
  void Start();
  void Stop();
 private:
  int sec_;
  int nsec_;
  bool is_repeating_;
 
  std::function<void()> callback_;
  
  static void handler(int sig, siginfo_t* si, void* uc);

  timer_t timerid_;
  struct itimerspec its_;
};

#endif  // TIMER_H_
