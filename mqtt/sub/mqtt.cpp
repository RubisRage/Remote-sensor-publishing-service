#include <cstdint>
#include <cstring>
#include <iostream>

#include "mqtt.hpp"

MQTT::MQTT(const char *host) { connect(host); }

MQTT::~MQTT() {}

void MQTT::on_connect(int rc) {
  if (rc == MOSQ_ERR_SUCCESS) {
    subscribe(NULL, "/srf02/1");
    subscribe(NULL, "/srf02/2");
  }
}

void MQTT::on_message(const struct mosquitto_message *message) {
  uint8_t buf[2];
  memcpy(buf, message->payload, 2 * sizeof(uint8_t));
  uint16_t measure = buf[0] << 8 | buf[1];
  std::cout << "Topic " << message->topic << ": " << measure << " cm\n";
}

void MQTT::on_subscribe(int mid, int qos_count, const int *granted_qos) {
  std::cout << "Subscription succeeded!" << '\n';
}
