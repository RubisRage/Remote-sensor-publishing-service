#pragma once

#include "config/lora_protocol.h"
#include "types/Message.hpp"
#include <Buffer.hpp>

extern Buffer<Message, CertSense::max_window_size> received_messages;

void receive_messages();
