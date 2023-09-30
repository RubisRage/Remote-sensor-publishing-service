#pragma once

#include "types/Message.hpp"
#include <Buffer.hpp>
#include <config/lora_protocol.h>

extern Buffer<Message, message_buffer_capacity> message_buffer;

void build_messages();
