#include "Arduino.h"
#include "LoRa.h"
#include "Logger.hpp"
#include "LoraHandler.hpp"
#include "range_forwarder.hpp"
#include "sender.hpp"
#include "transport_layer.hpp"

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
}

void loop() {
  /* Process received messages and update connection state */
  process_messages();

  /* Send enqueued messages */
  send_message();

  /* Forward received ranges */
  forward_ranges();
}