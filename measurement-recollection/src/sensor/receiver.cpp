#include "receiver.hpp"
#include "LoraHandler.hpp"

Buffer<Message, CertSense::max_window_size> received_messages;

void receive_messages() {
  loraHandler.storeMessage();

  if (loraHandler.hasBeenRead()) {
    return;
  }

  Message message = loraHandler.getMessage();

  received_messages.push(message);
}
