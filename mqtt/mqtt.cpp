#include <iostream>
#include <cstring>

#include "mqtt.hpp"

MQTT::MQTT(const char* host){
    connect(host);
}

MQTT::~MQTT(){}

void MQTT::on_connect(int rc){
    if(rc == MOSQ_ERR_SUCCESS){
        subscribe(NULL, "/srf02/1");
        subscribe(NULL, "/srf02/2");
    }
}

void MQTT::on_message(const struct mosquitto_message* message){
    char buf[2];
    memcpy(buf, message->payload, 2*sizeof(char));
    uint16_t measure = (uint8_t)buf[0] << 8 | (uint8_t)buf[1];
    std::cout << "Topic " << message->topic << ": " << measure << '\n';
}

void MQTT::on_subscribe(int mid, int qos_count, const int* granted_qos){
    std::cout << "Subsscription succeeded" << '\n';
}