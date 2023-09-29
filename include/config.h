#pragma once

#include <chrono>
#include <cstdint>

constexpr auto initialIntervalBetweenTx = std::chrono::milliseconds(10000);

constexpr uint8_t collectorAddress = 0xB0;

#ifdef COLLECTOR
constexpr uint8_t localAddress = collectorAddress;
#endif

#ifdef SENSOR1
constexpr uint8_t localAddress = 0xB1;
#endif

#ifdef SENSOR2
constexpr uint8_t localAddress = 0xB2;
#endif
