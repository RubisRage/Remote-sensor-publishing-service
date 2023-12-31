#pragma once

#include "types/LoRaConfig.hpp"
#include <array>
#include <config/address.h>
#include <cstdint>

constexpr std::array<const char *, 3> messageTypesNames = {"uninitialized",
                                                           "range", "control"};
/*
 * Payload format for message types (bytewise)
 *
 * Range:
 *
 * | range1 highbyte | range1 lowbyte | range2 highbyte | range2 lowbyte | ...
 *
 * Control:
 * | ACK (0) | next_seq highbyte | next_seq lowbyte
 * | ARQ (1) | seq1 highbyte | seq1 lowbyte | seq2 highbyte | seq2 lowbyte | ...
 *
 */

struct Message {
  enum Type : uint8_t { uninitialized, range, control };

  Message() = default;

  Message(uint16_t seq, uint8_t dstAddr, Type type)
      : seq(seq), type(type), sourceAddress(local_address),
        destinationAddress(dstAddr), payloadLength(0), payload() {}

  /* Header */
  uint16_t seq = 0;
  Type type = Type::uninitialized;
  uint8_t sourceAddress = 0;
  uint8_t destinationAddress = 0;
  uint8_t payloadLength = 0;

  static constexpr uint8_t header_size =
      sizeof(seq) + sizeof(type) + sizeof(sourceAddress) +
      sizeof(destinationAddress) + sizeof(payloadLength);

  static constexpr uint8_t max_payload = loraConfig.max_payload - header_size;

  /* Payload */
  std::array<uint8_t, max_payload> payload;
};
