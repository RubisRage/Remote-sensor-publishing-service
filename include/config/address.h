#pragma once

#include <chrono>
#include <cstdint>

constexpr uint8_t broadcast_address = 0xFF;
constexpr uint8_t collector_address = 0xB0;
constexpr uint8_t sensor1_address = 0xB1;
constexpr uint8_t sensor2_address = 0xB2;

#ifdef COLLECTOR
constexpr uint8_t local_address = collector_address;
#endif

#ifdef SENSOR1
constexpr uint8_t local_address = sensor1_address;
#endif

#ifdef SENSOR2
constexpr uint8_t local_address = sensor2_address;
#endif
