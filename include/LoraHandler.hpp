#pragma once

#include "sys/_stdint.h"
#include <DutyCycleManager.hpp>
#include <cstdint>
#include <types/LoRaConfig.hpp>
#include <types/Message.hpp>

class LoraHandler {
public:
  LoraHandler();
  LoraHandler(LoraHandler &&) = delete;
  LoraHandler(const LoraHandler &) = delete;
  LoraHandler &operator=(LoraHandler &&) = delete;
  LoraHandler &operator=(const LoraHandler &) = delete;
  ~LoraHandler() = default;

  /**
   * Initialization function. Must be called at least once before
   * using any other function.
   */
  void setup(const LoRaConfig &config);

  /**
   * Send message using LoRa library.
   *
   * @return true if message sent, false if duty cycle restriction would
   * be violated by transmission.
   */
  bool send(const Message &);

  /**
   * Update LoRa configuration parameters.
   */
  void updateConfig(const LoRaConfig &config);

  /**
   * @return true if transmission is possible, false otherwise. A transmission
   * is possible when a transmission is not currently happening and a duty
   * cycle of 1% would be respected if a transmission were to start at the
   * current moment
   */
  bool canTransmit();

  /**
   * Get last received message.
   */
  Message getMessage();
  bool hasBeenRead();

  /*
   * Read and store last received message
   */
  void storeMessage();

private:
  bool packetNotEnded(uint8_t receivedBytes, int packetSize,
                      uint8_t payload_length);

  Message lastReceived;
  bool _hasBeenRead;
  uint16_t last_seq;
  DutyCycleManager dutyCycleManager;
};

extern LoraHandler loraHandler;
