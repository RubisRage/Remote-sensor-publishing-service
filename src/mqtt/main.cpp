#include <iostream>

#include "mqtt.hpp"
#include "mosquittopp.h"

int main()
{
	MQTT broker{"localhost"};

	mosqpp::lib_init();

	broker.loop_forever();

	mosqpp::lib_cleanup();
}
