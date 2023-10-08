#include <Timer.hpp>

bool Timer::hasTimedOut(bool reset_on_timeout) {
  if (disabled)
    return false;

  if (!waitStarted) {
    waitStarted = true;
    waitStart = ArduinoClock::now();

    return false;
  }

  bool timedout = ArduinoClock::now() - waitStart > timeout;

  if (timedout && reset_on_timeout) {
    reset();
  }

  return timedout;
}
