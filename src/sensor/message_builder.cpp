#include "message_builder.hpp"
#include "Logger.hpp"
#include "config/address.h"
#include "config/lora_protocol.h"
#include "cstdint"
#include "range.hpp"

Buffer<Message, CertSense::message_buffer_capacity> message_buffer;
static uint16_t seq_number = 0;

void build_messages() {
  if (range_buffer.size() * sizeof(uint16_t) < Message::max_payload) {
    return;
  }

  if (message_buffer.size() == CertSense::message_buffer_capacity) {
    return;
  }

  constexpr size_t range_number = Message::max_payload / sizeof(uint16_t);

  Message message(seq_number++, collector_address, Message::Type::range);

  for (size_t i = 0; i < range_number; i++) {
    uint16_t range = *range_buffer[i];

    message.payload[2 * i] = range >> 8;
    message.payload[2 * i + 1] = range & 0x00FF;
  }

  message_buffer.push(message);
  range_buffer.pop(range_number);

  serial.log<Message, CertSense::message_buffer_capacity>(
      LogLevel::info, "Message buffer updated: ", message_buffer,
      [](const Message &msg) {
        Serial.print(msg.seq);
        Serial.print(" ");
      });
}
