#include <array>
#include <functional>
#include <iostream>

#include "mosquittopp.h"
#include "serial.hpp"
#include <algorithm>
#include <csignal>

constexpr char startByte = 254;
constexpr size_t payload_size = 20;

bool running = true;

void publish(const std::array<char, payload_size> &data) {

  const char *topic;

  switch (data[0]) {
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

  for (size_t i = 1; i + 1 < data.size(); i += 2) {
    std::pair<char, char> measure{data[i], data[i + 1]};

    if (int pub = mqtt.publish(NULL, topic, sizeof(measure), &measure);
        pub != MOSQ_ERR_SUCCESS) {
      std::cerr << "Error publishing message to topic: " << topic << '\n';
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
  std::array<char, payload_size> payload;
  auto payload_it = payload.begin();

  signal(SIGINT, [](int) { running = false; });

  while (running) {
    auto data = serialHandler.read();

    serialPortHandler::buffer_type::iterator start = data.begin();
    serialPortHandler::buffer_type::iterator stop;

    while (stop != data.end()) {
      if (payload_it == payload.begin()) {
        start = std::find(start, data.end(), startByte);
      }

      stop = std::min(start + (payload.end() - payload_it), data.end());

      payload_it = std::copy(start, stop, payload_it);
      start = stop;

      if (payload_it == payload.end()) {
        publish(payload);
        payload_it = payload.begin();
      }
    }
  }

  mosqpp::lib_cleanup();

  return 0;
}
