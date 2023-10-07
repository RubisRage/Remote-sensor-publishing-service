#pragma once

#include "ArduinoClock.hpp"

class Timer {
public:
  inline Timer(ArduinoClock::duration timeout)
      : waitStarted(false), timeout(timeout) {
    enable();
  }

  Timer() = default;
  ~Timer() = default;
  Timer(Timer &&) = delete;
  Timer(const Timer &) = delete;
  Timer &operator=(Timer &&) = delete;
  Timer &operator=(const Timer &) = delete;

  bool hasTimedOut(bool reset_on_timeout = true);

  inline void reset() { waitStarted = false; }
  inline void setTimeout(const ArduinoClock::duration &&timeout) {
    reset();
    enable();
    this->timeout = timeout;
  }

  inline void enable() { disabled = false; }

private:
  bool waitStarted;
  bool disabled = true;
  ArduinoClock::time_point waitStart;
  ArduinoClock::duration timeout;
};
