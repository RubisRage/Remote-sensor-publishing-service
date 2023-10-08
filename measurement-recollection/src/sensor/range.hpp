#pragma once

#include <Buffer.hpp>
#include <config/sensor.h>
#include <cstdint>

extern Buffer<uint16_t, range_buffer_capacity> range_buffer;

void init_sensor();
void store_range();
