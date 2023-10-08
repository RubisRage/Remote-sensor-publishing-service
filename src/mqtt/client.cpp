#include <iostream>
#include <array>

#include "mosquittopp.h"
#include "serial.hpp"

constexpr char startByte = 's';
constexpr char stopByte = 'e';
constexpr int packetSize = 4;

int main(){
   
	mosqpp::lib_init();
	mosqpp::mosquittopp mqtt{};

    int conn = mqtt.connect("127.0.0.1");
	if(conn == MOSQ_ERR_SUCCESS)
	{
		std::cout << "successfully connected to MQTT Broker" << '\n'; 
	}

    serialPortHandler serialHandler("/dev/ttyACM0");

    while(true){
        auto data = serialHandler.read();

        std::size_t n {1};

        for(std::size_t i = 0; i < data.size() - packetSize; i = i + n){
            n = 1;
            if(data[i] == startByte && data[i + packetSize] == stopByte){
                std::pair<char, char> measure {data[i + 2], data[i + 3]};
                const char* topic = data[i + 1] == '1' ? "/srf02/1" : "/srf02/2";
                if(int pub = mqtt.publish(NULL, topic, sizeof(measure), &measure);
		            pub != MOSQ_ERR_SUCCESS){
		            std::cout << "Error publishing message to topic " << topic << '\n';
                    continue; 	
	            }
                n = packetSize;
            }
        }
    }

    mosqpp::lib_cleanup();

    return 0;
}