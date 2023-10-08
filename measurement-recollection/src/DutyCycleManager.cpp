#include <DutyCycleManager/DutyCycleManager.hpp>
#include <Logger.hpp>
#include <chrono>

DutyCycleManager::DutyCycleManager(ArduinoClock::duration initialTxDelay)
    : lastTxBegin(), txBegin(), txDelay(initialTxDelay), dutyCycle() {}

void DutyCycleManager::updateIntervalBetweenTx() {
  ArduinoClock::duration txDuration = ArduinoClock::now() - txBegin;
  ArduinoClock::duration sinceLastMessage = txBegin - lastTxBegin;

  lastTxBegin = txBegin;
  dutyCycle = 100.0f * txDuration / sinceLastMessage;

  serial.log(LogLevel::statistics, "Tx completed in", (int)txDuration.count(),
             "ms");

  serial.log(LogLevel::statistics, "Duty cycle is", dutyCycle, "%");

  auto previousTxDelay = txDelay;

  if (dutyCycle > 1.0f) {
    txDelay = txDuration * 100;
    serial.log(LogLevel::warning, "Duty cycle exceeded 1%, adjusting. Was",
               previousTxDelay.count(), "ms, now is ", txDelay.count(), "ms");
  }

  if (txDuration < 0.008f * (txDelay + txDuration)) {
    txDelay = txDuration * 100;
    serial.log(LogLevel::info, "Duty cycle under 0.8%, adjusting. Was",
               previousTxDelay.count(), "ms, now is ", txDelay.count(), "ms");
  }
}

bool DutyCycleManager::canTransmit() {
  return (ArduinoClock::now() - lastTxBegin) > txDelay;
}

void DutyCycleManager::beginTx() { txBegin = ArduinoClock::now(); }
