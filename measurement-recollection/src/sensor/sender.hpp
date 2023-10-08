#include "Buffer.hpp"
#include <config/lora_protocol.h>
#include <types/Message.hpp>

extern Buffer<Message *, CertSense::max_window_size> message_queue;

void send_message();
