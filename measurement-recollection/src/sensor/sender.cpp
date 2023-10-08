#include "sender.hpp"
#include "LoraHandler.hpp"
#include "config/address.h"
#include "types/Message.hpp"

Buffer<Message *, CertSense::max_window_size> message_queue;

#define SENDER_MOCK

#ifdef SENDER_MOCK
static Message mock_message(0xAAAA, local_address & 0xf,
                            Message::Type::uninitialized);
#endif

void send_message() {
  if (message_queue.size() == 0) {
    return;
  }

  if (!loraHandler.canTransmit()) {
    return;
  }

  const Message *message;

#ifdef SENDER_MOCK

  int fail = random(1, 101);
  if (fail <= 10) {
    message = &mock_message;
  } else {
    message = *message_queue.peek();
  }
#else
  message = *message_queue.peek();
#endif

  loraHandler.send(*message);
  message_queue.pop();
}
