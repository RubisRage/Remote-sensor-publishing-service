#include "ConnectionManager.hpp"
#include "Logger.hpp"
#include "StateMachine.hpp"
#include "config/address.h"
#include "config/lora_protocol.h"
#include "sender.hpp"
#include "sensor/message_builder.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>

#define TRANSITION(mfunc) &mfunc, #mfunc

ConnectionManager::ConnectionManager(const char *id,
                                     const uint8_t sender_address)
    : id(id), sender_address(sender_address), payload_buffer(), timeout(),
      state_machine(id,
                    TRANSITION(ConnectionManager::handle_received_messages)) {
  message_buffer.allocate(CertSense::max_window_size);
  message_has_been_received.allocate(CertSense::max_window_size);
}

void ConnectionManager::manage_connection() {
  const auto state = state_machine.getState();
  (this->*state)();
}

Buffer<uint8_t, ConnectionManager::max_payload_throughput> &
ConnectionManager::get_payload_buffer() {
  return payload_buffer;
}

void ConnectionManager::store_message(const Message &message) {
  if (message.type != Message::Type::range) {
    serial.log(LogLevel::info, id,
               ": Received message's type is not range! Droping.");
    return;
  }

  size_t buffer_index = message.seq - window_start_seq;
  *message_buffer[buffer_index] = message;
  *message_has_been_received[buffer_index] = true;

  next_expected_seq = message.seq + 1;

  uint16_t remaining_messages =
      (window_start_seq + CertSense::max_window_size) - message.seq - 1;

  timeout.setTimeout(single_message_timeout * remaining_messages);

  serial.log(LogLevel::debug, id, ": Received message, wss: ", window_start_seq,
             ", nes: ", next_expected_seq, ", ntp: ", next_to_process_seq,
             ", seq: ", message.seq);
}

/* States */

void ConnectionManager::handle_received_messages() {

  size_t buffer_index = next_to_process_seq - window_start_seq;

  if (*message_has_been_received[buffer_index]) {
    const Message &to_process = *message_buffer[buffer_index];

    next_to_process_seq++;

    for (size_t i = 0; i < to_process.payloadLength; i++) {
      payload_buffer.push(to_process.payload[i]);
    }
  }

  bool completed_window = true;
  for (size_t i = 0; i < message_has_been_received.size(); i++) {
    completed_window &= *message_has_been_received[i];
  }

  if (completed_window) {
    state_machine.transition(
        TRANSITION(ConnectionManager::handle_completed_window));
    return;
  }

  if (next_expected_seq == (window_start_seq + CertSense::max_window_size)) {
    state_machine.transition(
        TRANSITION(ConnectionManager::handle_uncompleted_window));
    return;
  }

  if (timeout.hasTimedOut()) {
    serial.log(LogLevel::error, id, ": Timeout!\n");
    state_machine.transition(
        TRANSITION(ConnectionManager::handle_uncompleted_window));
    return;
  }
}

void ConnectionManager::handle_completed_window() {
  window_start_seq += CertSense::max_window_size;
  next_expected_seq = window_start_seq;
  next_to_process_seq = window_start_seq;

  message_buffer.pop(CertSense::max_window_size);
  message_has_been_received.pop(CertSense::max_window_size);

  message_buffer.allocate(CertSense::max_window_size);
  message_has_been_received.allocate(CertSense::max_window_size);

  for (size_t i = 0; i < message_has_been_received.size(); i++) {
    *message_has_been_received[i] = false;
  }

  Message &ack = *message_queue.allocate();
  ack.seq = own_seq++;
  ack.type = Message::Type::control;
  ack.sourceAddress = local_address;
  ack.destinationAddress = sender_address;
  ack.payloadLength = 3;
  ack.payload[0] = CertSense::ack;
  ack.payload[1] = next_expected_seq >> 8;
  ack.payload[2] = 0xff & next_expected_seq;

  serial.log(LogLevel::debug, id, ": Completed window, wss: ", window_start_seq,
             ", nes: ", next_expected_seq, ", ntp: ", next_to_process_seq,
             ", seq: ");

  timeout.setTimeout(single_message_timeout * CertSense::max_window_size);
  state_machine.transition(
      TRANSITION(ConnectionManager::handle_received_messages));
}

void ConnectionManager::handle_uncompleted_window() {
  Message &arq = *message_queue.allocate();
  arq.seq = own_seq++;
  arq.type = Message::Type::control;
  arq.sourceAddress = local_address;
  arq.destinationAddress = sender_address;
  arq.payload[0] = CertSense::arq;
  arq.payloadLength = 1;

  for (size_t i = 0; i < message_has_been_received.size(); i++) {
    if (!*message_has_been_received[i]) {
      uint16_t arq_seq = window_start_seq + i;

      arq.payload[arq.payloadLength] = arq_seq << 8;
      arq.payload[arq.payloadLength + 1] = 0xff & arq_seq;

      arq.payloadLength += 2;
    }
  }

  uint16_t acked = next_to_process_seq - window_start_seq;

  window_start_seq = next_to_process_seq;
  next_expected_seq = next_to_process_seq;

  message_buffer.pop(acked);
  message_has_been_received.pop(acked);

  message_buffer.allocate(acked);
  message_has_been_received.allocate(acked);

  for (size_t i = CertSense::max_window_size - acked;
       i < message_has_been_received.size(); i++) {
    *message_has_been_received[i] = false;
  }

  serial.log(LogLevel::debug, id,
             ": Uncompleted window, wss: ", window_start_seq,
             ", nes: ", next_expected_seq, ", ntp: ", next_to_process_seq,
             ", mq_size: ", message_queue.size());

  timeout.setTimeout(single_message_timeout * CertSense::max_window_size);
  state_machine.transition(
      TRANSITION(ConnectionManager::handle_received_messages));
}
