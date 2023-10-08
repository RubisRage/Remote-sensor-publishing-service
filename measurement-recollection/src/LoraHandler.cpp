#include "config/lora_protocol.h"
#include "types/Message.hpp"
#include <LoRa.h>
#include <Logger.hpp>
#include <LoraHandler.hpp>
#include <cstring>
#include <stdlib.h>

LoraHandler::LoraHandler()
    : lastReceived(), _hasBeenRead(true), last_seq(CertSense::first_seq - 1),
      dutyCycleManager(initial_interval_between_tx) {}

bool LoraHandler::send(const Message &message) {
  if (!dutyCycleManager.canTransmit())
    return false;

  dutyCycleManager.beginTx();

  while (!LoRa.beginPacket()) {
    delay(10);
  }

  LoRa.write(message.destinationAddress);
  LoRa.write(message.sourceAddress);
  LoRa.write((uint8_t)(message.seq >> 8));
  LoRa.write((uint8_t)(message.seq & 0xFF));
  LoRa.write(message.type);
  LoRa.write(message.payloadLength);
  // LoRa.write(message.payload.data(), (size_t)message.payloadLength);

  for (uint8_t byte : message.payload) {
    LoRa.write(byte);
  }

  LoRa.endPacket();

  serial.log(LogLevel::info, "Sending message:", message);
  dutyCycleManager.updateIntervalBetweenTx();

  return true;
}

inline bool LoraHandler::packetNotEnded(uint8_t receivedBytes, int packetSize,
                                        uint8_t payload_length) {
  return receivedBytes <= uint8_t(lastReceived.payload.size() - 1) &&
         receivedBytes < payload_length &&
         Message::header_size + receivedBytes < packetSize;
}

void LoraHandler::storeMessage() {

  int packetSize = LoRa.parsePacket();

  if (packetSize == 0)
    return;

  Message packet;

  packet.destinationAddress = LoRa.read();
  packet.sourceAddress = LoRa.read();
  packet.seq = ((uint16_t)LoRa.read() << 8) | (uint16_t)LoRa.read();
  packet.type = Message::Type(LoRa.read());
  packet.payloadLength = LoRa.read();

  uint8_t receivedBytes = 0;
  while (packetNotEnded(receivedBytes, packetSize, packet.payloadLength)) {
    packet.payload[receivedBytes++] = (uint8_t)LoRa.read();
  }

  if (packet.payloadLength != receivedBytes) {
    serial.log(LogLevel::debug, "Receiving error: declared message length ",
               lastReceived.payloadLength, " does not match length ",
               receivedBytes);
    packet.payloadLength = 0;

    serial.log(LogLevel::error, "Received message (Payload omitted)",
               lastReceived);

    return;
  }

  if (packet.destinationAddress != local_address &&
      packet.destinationAddress != broadcast_address) {
    serial.log(
        LogLevel::warning,
        "Received message was not meant for local node, destination address:",
        packet.destinationAddress, ", dropping.");

    return;
  }

  if (last_seq == packet.seq &&
      lastReceived.sourceAddress == packet.sourceAddress) {
    serial.log(LogLevel::error, "Received duplicate message, dropping.");
    return;
  }

  serial.log(LogLevel::info, "Received message:", packet);

  lastReceived = packet;
  last_seq = lastReceived.seq;
  _hasBeenRead = false;
}

void LoraHandler::setup(const LoRaConfig &config) {
  LoRa.setSyncWord(0x12);
  LoRa.setPreambleLength(8);

  updateConfig(config);
}

void LoraHandler::updateConfig(const LoRaConfig &config) {
  LoRa.setSignalBandwidth(long(bandwidth_kHz[config.bandwidthIndex]));
  LoRa.setSpreadingFactor(config.spreadingFactor);
  LoRa.setCodingRate4(config.codingRate);
  LoRa.setTxPower(config.txPower, PA_OUTPUT_PA_BOOST_PIN);
}

bool LoraHandler::canTransmit() { return dutyCycleManager.canTransmit(); }

Message LoraHandler::getMessage() {
  _hasBeenRead = true;
  return lastReceived;
}

bool LoraHandler::hasBeenRead() { return _hasBeenRead; }

/* Global definitions (extern) */
LoraHandler loraHandler;
