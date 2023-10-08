#pragma once

#include <ArduinoClock.hpp>
#include <Srf02.hpp>
#include <config/lora_protocol.h>
#include <cstddef>
#include <cstdint>
#include <types/LoRaConfig.hpp>
#include <types/Message.hpp>

constexpr Srf02::Unit sensor_unit = Srf02::Unit::cm;
constexpr ArduinoClock::duration sensor_read_period =
    std::chrono::milliseconds(100);

constexpr size_t ranges_per_message = Message::max_payload / sizeof(uint16_t);
constexpr size_t time_to_ack_ms =
    (CertSense::max_window_size + 1) * loraConfig.receive_delay_ms;

constexpr float ranges_throughput_per_ms =
    float(CertSense::max_window_size * ranges_per_message) / time_to_ack_ms;
constexpr float ranges_generated_per_ms = 1.0 / sensor_read_period.count();

constexpr size_t range_buffer_capacity = 512;

#ifdef SENSOR1
constexpr uint8_t sensor_address = 0xE0;
#endif

#ifdef SENSOR2
constexpr uint8_t sensor_address = 0xE0;
#endif
