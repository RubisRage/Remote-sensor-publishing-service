#include "sender.hpp"
#include "LoraHandler.hpp"

Buffer<Message, number_of_sensor_devices> message_queue;

void send_message() {
  if (message_queue.size() == 0) {
    return;
  }

  if (!loraHandler.canTransmit()) {
    return;
  }

  loraHandler.send(*message_queue.peek());
  message_queue.pop();
}
