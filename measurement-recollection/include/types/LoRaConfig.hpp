#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

struct LoRaConfig {
  uint8_t bandwidthIndex;
  uint8_t spreadingFactor;
  uint8_t codingRate;
  uint8_t txPower;
  size_t max_payload;
  uint32_t receive_delay_ms;
};

constexpr uint32_t tx_time = 41;
// constexpr LoRaConfig loraConfig = {8, 7, 1, 2, 235, tx_time + tx_time * 100};
constexpr LoRaConfig loraConfig = {8, 7, 1, 2, 26, tx_time + tx_time * 100};

/* Possible bandwidths */
constexpr std::array<double, 10> bandwidth_kHz = {
    7.8E3,  10.4E3, 15.6E3, 20.8E3, 31.25E3,
    41.7E3, 62.5E3, 125E3,  250E3,  500E3};
