#include <ArduinoClock.hpp>
#include <cstdint>

constexpr auto initial_interval_between_tx = std::chrono::milliseconds(10000);

/**
 *  Updates interval between transmissions to ensure a duty cycle under 1%
 */
class DutyCycleManager {
public:
  DutyCycleManager(ArduinoClock::duration initialTxDelay);
  DutyCycleManager(DutyCycleManager &&) = delete;
  DutyCycleManager(const DutyCycleManager &) = delete;
  DutyCycleManager &operator=(DutyCycleManager &&) = delete;
  DutyCycleManager &operator=(const DutyCycleManager &) = delete;
  ~DutyCycleManager() = default;

  void updateIntervalBetweenTx();
  void beginTx();
  bool canTransmit();

  // private:
  ArduinoClock::time_point lastTxBegin;
  ArduinoClock::time_point txBegin;
  ArduinoClock::duration txDelay;
  float dutyCycle;
};
