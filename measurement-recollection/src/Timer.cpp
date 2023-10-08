#include "DutyCycleManager/ArduinoClock.hpp"
#include <Timer.hpp>

bool Timer::hasTimedOut(bool reset_flag) {
  if (!waitStarted) {
    waitStarted = true;
    waitStart = ArduinoClock::now();

    return false;
  }

  bool timedout = ArduinoClock::now() - waitStart > timeout;

  if (timedout && reset_flag) {
    reset();
  }

  return timedout;
}
