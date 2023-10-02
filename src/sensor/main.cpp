#include "Arduino.h"
#include "LoRa.h"
#include "LoraHandler.hpp"
#include "Srf02.hpp"
#include "Timer.hpp"
#include "range.hpp"
#include "sender.hpp"
#include "sensor/message_builder.hpp"
#include "sensor/range.hpp"
#include "sensor/receiver.hpp"
#include "sensor/transport_layer.hpp"
#include <Logger.hpp>
#include <algorithm>

void setup() {
  Serial.begin(115200);

  while (!Serial)
    ;

  if (!LoRa.begin(868E6)) {
    serial.log(LogLevel::failure, "LoRa init failed. Check your connections.");
    while (1)
      ;
  }

  loraHandler.setup(loraConfig);

  init_sensor();
}

Timer log_timer(std::chrono::milliseconds(3000));

void loop() {
  /* Store measurments */
  store_range();

  /* Store received messages */
  receive_messages();

  /* Build messages */
  build_messages();

  /* Process built messages and received ACKs */
  process_messages();

  /* Send messages */
  send_message();

  if (log_timer.hasTimedOut()) {
    serial.log(LogLevel::info, "Range buffer:", range_buffer.size());
    serial.log(LogLevel::info, "Message buffer:", message_buffer.size());
    serial.log(LogLevel::info, "Message queue:", message_queue.size());
  }
}
