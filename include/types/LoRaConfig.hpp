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

constexpr LoRaConfig loraConfig = {8, 7, 1, 2, 248};

/* Possible bandwidths */
constexpr std::array<double, 10> bandwidth_kHz = {
    7.8E3,  10.4E3, 15.6E3, 20.8E3, 31.25E3,
    41.7E3, 62.5E3, 125E3,  250E3,  500E3};
