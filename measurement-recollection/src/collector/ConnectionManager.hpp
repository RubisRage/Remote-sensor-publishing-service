#pragma once

#include "ArduinoClock.hpp"
#include "Buffer.hpp"
#include "StateMachine.hpp"
#include "Timer.hpp"
#include "config/lora_protocol.h"
#include "types/Message.hpp"
#include <cstddef>
#include <cstdint>

class ConnectionManager {
public:
  ConnectionManager(const char *id, const uint8_t sender_address);
  ConnectionManager(ConnectionManager &&);
  ~ConnectionManager() = default;

  ConnectionManager(const ConnectionManager &) = delete;
  ConnectionManager &operator=(ConnectionManager &&) = delete;
  ConnectionManager &operator=(const ConnectionManager &) = delete;

  static constexpr size_t max_payload_throughput =
      CertSense::max_window_size * Message::max_payload;

  void manage_connection();
  void store_message(const Message &message);
  Buffer<uint8_t, max_payload_throughput> &get_payload_buffer();

  const char *const id;
  const uint8_t sender_address;

  /* Data buffers */
private:
  Buffer<uint8_t, max_payload_throughput> payload_buffer;
  Buffer<Message, CertSense::max_window_size> message_buffer;

  /* Conexion management */
private:
  uint16_t own_seq = CertSense::first_seq;
  uint16_t window_start_seq = CertSense::first_seq;
  uint16_t next_to_process_seq = CertSense::first_seq;
  uint16_t next_expected_seq = CertSense::first_seq;
  Buffer<bool, CertSense::max_window_size> message_has_been_received;
  Timer timeout;
  static constexpr auto single_message_timeout = std::chrono::milliseconds(
      int32_t(CertSense::timeout_scale * loraConfig.receive_delay_ms));

  /* States */
private:
  typedef void (ConnectionManager::*state_mfunc)(void);
  StateMachine<state_mfunc> state_machine;

  void handle_received_messages();
  void handle_completed_window();
  void handle_uncompleted_window();
};
