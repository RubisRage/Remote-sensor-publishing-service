#include "types/Message.hpp"
#include <LoRa.h>
#include <Logger.hpp>
#include <LoraHandler.hpp>
#include <config.h>
#include <cstring>
#include <stdlib.h>

LoraHandler::LoraHandler()
    : lastReceived(), _hasBeenRead(true),
      dutyCycleManager(initialIntervalBetweenTx) {}

bool LoraHandler::send(Message &message) {
  if (!dutyCycleManager.canTransmit())
    return false;

  dutyCycleManager.beginTx();

  while (!LoRa.beginPacket()) {
    delay(10);
  }

  LoRa.write(message.destinationAddress);
  LoRa.write(message.sourceAddress);
  LoRa.write((uint8_t)(message.id >> 8));
  LoRa.write((uint8_t)(message.id & 0xFF));
  LoRa.write(message.type);
  LoRa.write(message.payloadLength);
  LoRa.write(message.payload.data(), (size_t)message.payloadLength);

  LoRa.endPacket();

  serial.log(LogLevel::info, "Sending message:", message);
  dutyCycleManager.updateIntervalBetweenTx();

  return true;
}

inline bool LoraHandler::packetNotEnded(uint8_t receivedBytes, int packetSize) {
  return receivedBytes <= uint8_t(payload.size() - 1) &&
         receivedBytes < lastReceived.payloadLength &&
         Message::header_size + receivedBytes < packetSize;
}

void LoraHandler::storeMessage() {

  int packetSize = LoRa.parsePacket();

  if (packetSize == 0)
    return;

  lastReceived.destinationAddress = LoRa.read();
  lastReceived.sourceAddress = LoRa.read();
  lastReceived.id = ((uint16_t)LoRa.read() << 8) | (uint16_t)LoRa.read();
  lastReceived.type = Message::Type(LoRa.read());
  lastReceived.payloadLength = LoRa.read();

  uint8_t receivedBytes = 0;
  while (packetNotEnded(receivedBytes, packetSize)) {
    payload[receivedBytes++] = (uint8_t)LoRa.read();
  }

  if (lastReceived.payloadLength != receivedBytes) {
    serial.log(LogLevel::debug, "Receiving error: declared message length ",
               lastReceived.payloadLength, " does not match length ",
               receivedBytes);
    lastReceived.payloadLength = 0;

    serial.log(LogLevel::error, "Received message (Payload omitted)",
               lastReceived);

    return;
  }

  if ((lastReceived.destinationAddress & localAddress) != localAddress) {
    serial.log(
        LogLevel::warning,
        "Received message was not meant for local node, destination address:",
        lastReceived.destinationAddress, ", dropping.");

    return;
  }

  serial.log(LogLevel::info, "Received message:", lastReceived);

  _hasBeenRead = false;
}

void LoraHandler::setup(const LoRaConfig &config, void (*onReceive)(int)) {
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
