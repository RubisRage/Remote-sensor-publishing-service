#include "ConnectionManager.hpp"
#include "StateMachine.hpp"
#include "config/lora_protocol.h"
#include "sender.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>

#define TRANSITION(mfunc) &mfunc, #mfunc

ConnectionManager::ConnectionManager(const char *id,
                                     const uint8_t sender_address)
    : id(id), sender_address(sender_address),
      timeout(single_message_timeout * max_window_size),
      state_machine(id,
                    TRANSITION(ConnectionManager::handle_received_messages)) {}

void ConnectionManager::manage_connection() {
  const auto state = state_machine.getState();
  (this->*state)();
}

void ConnectionManager::store_message(const Message &message) {
  if (message.type != Message::Type::range) {
    serial.log(LogLevel::info, id,
               ": Received message's type is not range! Droping.");
    return;
  }

  uint16_t buffer_index = message.seq - window_start_seq;

  if (buffer_index < 0 || buffer_index > max_window_size) {
    serial.log(LogLevel::info, id,
               ": Received message's seq is out of window! Droping.");
    return;
  }

  message_has_been_received[buffer_index] = true;
  message_buffer[buffer_index] = message;
  next_expected_seq = message.seq + 1;

  uint16_t remaining_messages = next_expected_seq - window_start_seq;
  timeout.setTimeout(single_message_timeout * remaining_messages);
}

const Buffer<uint8_t, ConnectionManager::max_payload_throughput>
ConnectionManager::get_payload_buffer() {
  Buffer<uint8_t, ConnectionManager::max_payload_throughput> payload;

  for (size_t i = 0; i < payload_buffer.size(); i++) {
    payload.push(*payload_buffer[i]);
  }

  return payload;
}

/* States */

void ConnectionManager::handle_received_messages() {
  if (timeout.hasTimedOut()) {
    serial.log(LogLevel::info, id, ": Timeout!");
    state_machine.transition(
        TRANSITION(ConnectionManager::handle_uncompleted_window));
    return;
  }

  if (next_expected_seq == window_start_seq + max_window_size) {
    bool window_is_completed =
        std::all_of(message_has_been_received.begin(),
                    message_has_been_received.end(), [](bool b) { return b; });

    if (window_is_completed) {
      state_machine.transition(
          TRANSITION(ConnectionManager::handle_completed_window));
    } else {
      state_machine.transition(
          TRANSITION(ConnectionManager::handle_uncompleted_window));
    }

    return;
  }

  if (message_buffer.size() == 0)
    return;

  if (!message_has_been_received[next_to_process_seq]) {
    return;
  }

  const Message &message = message_buffer[next_to_process_seq];

  for (size_t i = 0; i < message.payloadLength; i++) {
    payload_buffer.push(message.payload[i]);
  }
}

void ConnectionManager::handle_completed_window() {
  window_start_seq += max_window_size;
  next_to_process_seq = window_start_seq;
  next_expected_seq = window_start_seq;

  std::fill(message_has_been_received.begin(), message_has_been_received.end(),
            false);

  Message &ack = *message_queue.allocate();
  ack.seq = own_seq++;
  ack.type = Message::Type::control;
  ack.sourceAddress = local_address;
  ack.destinationAddress = sender_address;
  ack.payloadLength = 2;
  ack.payload[0] = next_expected_seq << 8;
  ack.payload[1] = 0xff & next_expected_seq;

  state_machine.transition(
      TRANSITION(ConnectionManager::handle_received_messages));
}

void ConnectionManager::handle_uncompleted_window() {}
