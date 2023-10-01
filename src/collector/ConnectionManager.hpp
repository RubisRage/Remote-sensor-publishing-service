#pragma once

#include "Buffer.hpp"
#include "StateMachine.hpp"
#include "config/lora_protocol.h"
#include "types/Message.hpp"

class ConnectionManager {
public:
  ConnectionManager(const char *id);
  ConnectionManager(ConnectionManager &&);
  ~ConnectionManager() = default;

  ConnectionManager(const ConnectionManager &) = delete;
  ConnectionManager &operator=(ConnectionManager &&) = delete;
  ConnectionManager &operator=(const ConnectionManager &) = delete;

  static constexpr size_t max_payload_throughput =
      max_window_size * Message::max_payload;

  void manage_connection();
  void store_message(const Message &message);
  const Buffer<uint8_t, max_payload_throughput> get_payload_buffer();

  /* Fields */
  const char *const id;

private:
  StateMachine<void (ConnectionManager::*)(void)> state_machine;
  Buffer<uint8_t, max_payload_throughput> payload_buffer;
  Buffer<Message, max_window_size> message_buffer;

  /* States */
private:
  void initial_state();
};
