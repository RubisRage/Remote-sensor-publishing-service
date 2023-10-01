#pragma once

#include "Buffer.hpp"
#include "config/collector.h"
#include "types/Message.hpp"

extern Buffer<Message, number_of_sensor_devices> message_queue;

void send_message();
