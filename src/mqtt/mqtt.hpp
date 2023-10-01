#pragma once

#include "mosquittopp.h"

class MQTT : public mosqpp::mosquittopp {
    public:
        MQTT(const char* host);
        ~MQTT();

        void on_connect(int rc);
        void on_message(const struct mosquitto_message* message);
        void on_subscribe(int mid, int qos_count, const int* granted_qos);
};