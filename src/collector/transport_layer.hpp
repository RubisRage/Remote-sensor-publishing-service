#pragma once

#include "ConnectionManager.hpp"
#include "config/collector.h"

extern std::array<ConnectionManager, number_of_sensor_devices>
    connection_managers;

void process_messages();
