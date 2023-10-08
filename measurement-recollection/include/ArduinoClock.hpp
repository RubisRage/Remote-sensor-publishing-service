#pragma once

#include <Arduino.h>
#include <chrono>
#include <ratio>

struct ArduinoClock {
  // typedef std::chrono::milliseconds duration;
  typedef std::chrono::duration<unsigned long, std::ratio<1, 1000>> duration;
  typedef duration::rep rep;
  typedef duration::period period;
  typedef std::chrono::time_point<ArduinoClock> time_point;
  static constexpr bool is_steady = true;

  static time_point now() {
    return time_point(ArduinoClock::duration{millis()});
  }
};
