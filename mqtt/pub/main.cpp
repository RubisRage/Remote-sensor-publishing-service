#include <array>
#include <functional>
#include <iostream>

#include "mosquittopp.h"
#include "serial.hpp"
#include <algorithm>
#include <csignal>
#include <iterator>

constexpr uint8_t start_byte = 254;
constexpr uint8_t end_byte = 253;
constexpr size_t max_measurement_throughput = 3 * 20;

bool running = true;

void publish(mosqpp::mosquittopp &mqtt, const std::vector<uint8_t> &payload) {

  std::cout << "Publishing message with size: " << payload.size() << '\n';

  const char *topic;

  switch (payload[0]) {
  case 1:
    topic = "/srf02/1";
    break;
  case 2:
    topic = "/srf02/2";
    break;
  default:
    std::cerr << "Received unknown sensor id!" << '\n';
    return;
  }

  for (size_t i = 1; i + 1 < payload.size(); i += 2) {
    std::pair<uint8_t, uint8_t> measure{payload[i], payload[i + 1]};

    std::cout << "Publishing to topic: " << topic
              << " value: " << uint16_t(measure.first << 8 | measure.second)
              << '\n';

    if (int pub = mqtt.publish(NULL, topic, sizeof(measure), &measure);
        pub != MOSQ_ERR_SUCCESS) {
      std::cerr << "Error publishing message to topic: " << pub << '\n';
    }
  }
}

int main() {

  mosqpp::lib_init();
  mosqpp::mosquittopp mqtt{};

  int conn = mqtt.connect("127.0.0.1");
  if (conn == MOSQ_ERR_SUCCESS) {
    std::cout << "Successfully connected to MQTT Broker!" << '\n';
  }

  serialPortHandler serialHandler("/dev/ttyACM0");
  std::vector<uint8_t> payload;

  payload.reserve(max_measurement_throughput);

  auto payload_it = payload.begin();

  signal(SIGABRT, [](int) { running = false; });

  while (running) {
    auto data = serialHandler.read();

    serialPortHandler::buffer_type::iterator start = data.begin();
    serialPortHandler::buffer_type::iterator stop = data.begin();

    while (stop != data.end()) {
      if (payload.size() == 0) {
        start = std::find(start, data.end(), start_byte) + 1;
        if (start == data.end()) {
          break;
        }
      }

      stop = std::find(start, data.end(), end_byte);

      std::copy(start, stop, std::back_inserter(payload));

      start = stop;

      if (stop != data.end()) {
        publish(mqtt, payload);
        payload.clear();
      }
    }
  }

  mosqpp::lib_cleanup();

  return 0;
}
