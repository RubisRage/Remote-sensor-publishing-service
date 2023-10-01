#pragma once

#include <ArduinoClock.hpp>
#include <Srf02.hpp>
#include <cstddef>
#include <cstdint>

constexpr Srf02::Unit sensor_unit = Srf02::Unit::cm;
constexpr size_t range_buffer_capacity = 512;
constexpr ArduinoClock::duration sensor_read_period =
    std::chrono::milliseconds(100);

#ifdef SENSOR1
constexpr uint8_t sensor_address = 0xE0;
#endif

#ifdef SENSOR2
constexpr uint8_t sensor_address = 0xE0;
#endif
