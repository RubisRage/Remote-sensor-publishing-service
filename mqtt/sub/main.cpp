#include <csignal>
#include <iostream>

#include "mosquittopp.h"
#include "mqtt.hpp"

int main() {
  MQTT broker{"localhost"};

  mosqpp::lib_init();

  broker.loop_forever();

  mosqpp::lib_cleanup();
}
