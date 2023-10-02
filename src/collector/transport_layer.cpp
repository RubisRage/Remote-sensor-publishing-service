#include "transport_layer.hpp"
#include "LoraHandler.hpp"

std::array<ConnectionManager, number_of_sensor_devices> connection_managers = {
    ConnectionManager("sensor1", sensor1_address),
    ConnectionManager("sensor2", sensor2_address),
};

void process_messages() {
  if (!loraHandler.hasBeenRead()) {
    Message msg = loraHandler.getMessage();

    uint8_t device_address = 0xf & msg.sourceAddress;

    if (device_address < number_of_sensor_devices) {
      connection_managers[device_address].store_message(msg);
    } else {
      serial.log(LogLevel::warning,
                 "Received message from unknown device! Device address: ",
                 msg.sourceAddress);
    }
  }

  for (auto &connection_manager : connection_managers) {
    connection_manager.manage_connection();
  }
}
