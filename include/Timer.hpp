#pragma once

#include "DutyCycleManager/ArduinoClock.hpp"

class Timer {
public:
  inline Timer(ArduinoClock::duration timeout)
      : waitStarted(false), timeout(timeout) {}

  Timer() = default;
  ~Timer() = default;
  Timer(Timer &&) = delete;
  Timer(const Timer &) = delete;
  Timer &operator=(Timer &&) = delete;
  Timer &operator=(const Timer &) = delete;

  bool hasTimedOut(bool reset_flag = true);

  inline void reset() { waitStarted = false; }
  inline void setTimeout(const ArduinoClock::duration &&timeout) {
    this->timeout = timeout;
  }

private:
  bool waitStarted;
  ArduinoClock::time_point waitStart;
  ArduinoClock::duration timeout;
};
