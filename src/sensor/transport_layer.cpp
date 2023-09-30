#include "transport_layer.hpp"
#include "Logger.hpp"
#include "sender.hpp"
#include "sensor/message_builder.hpp"
#include "sensor/receiver.hpp"
#include <Buffer.hpp>
#include <StateMachine.hpp>
#include <algorithm>
#include <config/lora_protocol.h>
#include <iterator>

namespace States {
void queue_unsent_msgs();
void update_tx_window();
} // namespace States
extern StateMachine state_machine;

Buffer<size_t, max_window_size> unsent_messages;
size_t next_expected_ack = 0;
size_t current_window_size = 0;

void States::queue_unsent_msgs() {
  if (received_messages.size() != 0) {
    state_machine.transition(TRANSITION(States::update_tx_window));
    return;
  }

  for (size_t seq = next_expected_ack + current_window_size;
       current_window_size < message_buffer.size() &&
       current_window_size < max_window_size;
       seq++) {
    unsent_messages.push(seq);
    current_window_size++;
    serial.log<size_t, max_window_size>(LogLevel::info,
                                        "Unsent messages: ", unsent_messages,
                                        [](const size_t &seq) {
                                          Serial.print(seq);
                                          Serial.print(" ");
                                        });
  }

  for (size_t i = 0; i < unsent_messages.size(); i++) {
    size_t buffer_index = *unsent_messages[i] - next_expected_ack;

    message_queue.push(message_buffer[buffer_index]);
    unsent_messages.pop();
  }
}

void States::update_tx_window() {
  Message &message = *received_messages.peek();
  received_messages.pop();

  if (message.type != Message::Type::ack) {
    serial.log(LogLevel::info, "Received message type is not ack! Droping.");
    return;
  }

  if (message.payloadLength == 0) {
    next_expected_ack += current_window_size;
  }

  uint16_t min_acked_seq = message.payload[0] << 8 | message.payload[1];

  for (size_t i = 0; i < message.payloadLength; i += 2) {
    uint16_t seq = message.payload[i] << 8 | message.payload[i + 1];

    if (seq <= min_acked_seq) {
      min_acked_seq = seq;
    }

    unsent_messages.push(seq);
  }

  if (min_acked_seq > next_expected_ack) {
    next_expected_ack = min_acked_seq;
  }

  state_machine.transition(TRANSITION(States::queue_unsent_msgs));
}

StateMachine state_machine(TRANSITION(States::queue_unsent_msgs));

void process_messages() { state_machine.getState()(); }
