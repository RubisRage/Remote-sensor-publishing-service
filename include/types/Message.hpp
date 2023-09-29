#pragma once

#include "types/LoRaConfig.hpp"
#include <array>
#include <config.h>
#include <cstdint>

constexpr std::array<const char *, 3> messageTypesNames = {"unititialized",
                                                           "range", "ack"};

struct Message {
  enum Type : uint8_t { uninitialized, range, ack };

  Message() = default;

  Message(uint16_t id, uint8_t dstAddr, Type type)
      : id(id), type(type), sourceAddress(localAddress),
        destinationAddress(dstAddr), payloadLength(0), payload() {}

  /* Header */
  uint16_t id = 0;
  Type type = Type::uninitialized;
  uint8_t sourceAddress = 0;
  uint8_t destinationAddress = 0;
  uint8_t payloadLength = 0;

  static constexpr uint8_t header_size = sizeof(id) + sizeof(type) +
                                         sizeof(sourceAddress) +
                                         sizeof(destinationAddress);

  static constexpr uint8_t max_payload = loraConfig.max_payload - header_size;

  /* Payload */
  std::array<uint8_t, max_payload> payload;
};
