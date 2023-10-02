#pragma once
#include <chrono>

namespace CertSense {
constexpr size_t message_buffer_capacity = 10;
constexpr size_t max_window_size = 3;
constexpr size_t first_seq = 0;
constexpr double timeout_scale = 1.5;
constexpr uint8_t ack = 0;
constexpr uint8_t arq = 1;
} // namespace CertSense
