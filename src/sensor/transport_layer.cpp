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
#include <sys/_stdint.h>

#define TRANSITION(func) func, #func

namespace States {
void queue_unsent_msgs();
void handle_ack();
} // namespace States

extern StateMachine<void (*)(void)> state_machine;

Buffer<size_t, CertSense::max_window_size> unsent_messages;
uint16_t window_start_seq = CertSense::first_seq;
uint16_t current_window_size = 0;

void States::queue_unsent_msgs() {
  if (received_messages.size() != 0) {
    state_machine.transition(TRANSITION(States::handle_ack));
    return;
  }

  while (current_window_size < message_buffer.size() &&
         current_window_size < CertSense::max_window_size) {
    uint16_t seq = window_start_seq + current_window_size;

    unsent_messages.push(seq);
    current_window_size++;
  }

  for (size_t i = 0; i < unsent_messages.size(); i++) {
    size_t buffer_index = *unsent_messages.peek() - window_start_seq;
    message_queue.push(message_buffer[buffer_index]);
    unsent_messages.pop();
  }
}

void States::handle_ack() {
  const Message &message = *received_messages.peek();

  if (message.type != Message::Type::control) {
    serial.log(LogLevel::info,
               "Received message's type is not control! Droping.");
    state_machine.transition(TRANSITION(States::queue_unsent_msgs));
    return;
  }

  bool is_ack = message.payload[0] == CertSense::ack;

  size_t acked_messages;

  if (is_ack) {
    uint16_t seq = message.payload[1] << 8 | message.payload[2];
    acked_messages = seq - window_start_seq;
  } else {
    uint16_t min_arq_seq = message.payload[1] << 8 | message.payload[2];

    for (size_t i = 1; i + 1 < message.payloadLength; i += 2) {
      uint16_t seq = message.payload[i] << 8 | message.payload[i + 1];

      if (seq < min_arq_seq) {
        min_arq_seq = seq;
      }

      unsent_messages.push(seq);
    }

    acked_messages = min_arq_seq - window_start_seq;
  }

  message_buffer.pop(acked_messages);
  current_window_size -= acked_messages;
  window_start_seq += acked_messages;

  serial.log(LogLevel::debug, "Updated window: wss: ", window_start_seq,
             ", cws: ", current_window_size);

  received_messages.pop();
  state_machine.transition(TRANSITION(States::queue_unsent_msgs));
}

StateMachine<void (*)(void)>
    state_machine(TRANSITION(States::queue_unsent_msgs));

void process_messages() { state_machine.getState()(); }
