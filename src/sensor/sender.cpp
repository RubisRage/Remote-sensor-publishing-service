#include "sender.hpp"
#include "LoraHandler.hpp"

Buffer<Message *, CertSense::max_window_size> message_queue;

void send_message() {
  if (message_queue.size() == 0) {
    return;
  }

  if (!loraHandler.canTransmit()) {
    return;
  }

  loraHandler.send(**message_queue.peek());
  message_queue.pop();
}
