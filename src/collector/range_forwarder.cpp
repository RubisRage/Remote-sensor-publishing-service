#include "Arduino.h"
#include "transport_layer.hpp"

#define DEBUG

#ifdef DEBUG
#include "Timer.hpp"
#endif

void forward_ranges() {
  for (auto &connection_manager : connection_managers) {
    auto payload_buffer = connection_manager.get_payload_buffer();

#ifdef DEBUG
    static Timer forward_timer(std::chrono::milliseconds(2000));
    if (payload_buffer.size() > 0 && forward_timer.hasTimedOut()) {
#endif
      Serial.print("Forwarding payload(");
      Serial.print(connection_manager.id);
      for (size_t i = 0; i < payload_buffer.size(); i++) {
        Serial.print(*payload_buffer[i]);
        Serial.print(" ");
      }
      Serial.println();
#ifdef DEBUG
    }
#endif
  }
}
