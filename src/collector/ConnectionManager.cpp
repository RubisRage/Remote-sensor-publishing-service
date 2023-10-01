#include "ConnectionManager.hpp"
#include "StateMachine.hpp"
#include <algorithm>
#include <functional>

#define TRANSITION(mfunc) &mfunc, #mfunc

ConnectionManager::ConnectionManager(const char *id)
    : id(id), state_machine(id, TRANSITION(ConnectionManager::initial_state)) {}

void ConnectionManager::manage_connection() {
  const auto state = state_machine.getState();
  (this->*state)();
}

void ConnectionManager::store_message(const Message &message) {
  message_buffer.push(message);
}

void ConnectionManager::initial_state() {}

const Buffer<uint8_t, ConnectionManager::max_payload_throughput>
ConnectionManager::get_payload_buffer() {
  Buffer<uint8_t, ConnectionManager::max_payload_throughput> payload;

  for (size_t i = 0; i < payload_buffer.size(); i++) {
    payload.push(*payload_buffer[i]);
  }

  return payload;
}
