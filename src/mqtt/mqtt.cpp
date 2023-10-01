#include <iostream>

#include "mqtt.hpp"

MQTT::MQTT(const char* host){
    connect(host);
}

MQTT::~MQTT(){}

void MQTT::on_connect(int rc){
    if(rc == MOSQ_ERR_SUCCESS){
        subscribe(NULL, "/srf02/0");
        subscribe(NULL, "/srf02/1");
    }
}

void MQTT::on_message(const struct mosquitto_message* message){
    std::cout << "Message received: " << (char*)message->payload << '\n';
}

void MQTT::on_subscribe(int mid, int qos_count, const int* granted_qos){
    std::cout << "Subsscription succeeded" << '\n';
}