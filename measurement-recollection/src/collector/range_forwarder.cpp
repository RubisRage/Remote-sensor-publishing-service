#include "Arduino.h"
#include "transport_layer.hpp"
#include <sys/_stdint.h>

constexpr uint8_t start_byte = 254;
constexpr uint8_t end_byte = 253;

void forward_ranges() {
  for (auto &connection_manager : connection_managers) {
    auto &payload_buffer = connection_manager.get_payload_buffer();

    if (payload_buffer.size() == 0) {
      continue;
    }

    serial.log(LogLevel::info, connection_manager.id,
               ": Forwarding measurements! (", payload_buffer.size(), ")");

    Serial.write(start_byte);
    Serial.write(connection_manager.sender_address & 0xf);
    for (size_t i = 0; i < payload_buffer.size(); i++) {
      Serial.write(*payload_buffer[i]);
    }
    Serial.write(end_byte);

    Serial.println();

    serial.log(LogLevel::debug,
               "Cleared: ", payload_buffer.pop(payload_buffer.size()));
    serial.log(LogLevel::debug, "Size: ", payload_buffer.size());
  }
}
