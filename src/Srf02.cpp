#include "Srf02.hpp"
#include <Wire.h>

bool Srf02::initialized = false;

Srf02::Srf02(uint8_t address, uint8_t sensorId)
    : address_(address >> 1), unit_(Srf02::Unit::cm),
      last_measurement_ms_(millis()) {}

Srf02::~Srf02() {}

Srf02::Status Srf02::begin() {
  if (!Srf02::initialized) {
    Wire.begin();

    Srf02::initialized = true;

    return Srf02::Status::ok;
  }

  return Srf02::Status::already_initialized;
}

void Srf02::setUnit() {
  Wire.beginTransmission(address_);
  Wire.write(Srf02::Register::command_register);
  Wire.write(unit_);
  Wire.endTransmission();
}

Srf02::Status Srf02::startMeasurement() {
  if (sinceLastMeasurement() < delay_ms_)
    return Srf02::Status::delay_not_acomplished;

  setUnit();
  last_measurement_ms_ = millis();
  return Srf02::Status::ok;
}

Srf02::Status Srf02::readRange(uint16_t &range) {

  if (sinceLastMeasurement() < delay_ms_)
    return Srf02::Status::delay_not_acomplished;

  uint8_t range_high, range_low;

  if (read_register(Srf02::Register::range_high_byte, range_high) ||
      read_register(Srf02::Register::range_low_byte, range_low))
    return Srf02::Status::timeout;

  range = range_high << 8 | range_low;

  return Srf02::Status::ok;
}

int Srf02::read_register(Srf02::Register srf02_register, uint8_t &out) {
  Wire.beginTransmission(address_);
  Wire.write(srf02_register);
  Wire.endTransmission();

  unsigned long startTime_ms = millis();
  unsigned long timeout_ms = 500;

  Wire.requestFrom(address_, 1);

  while (!Wire.available()) {
    /* Wait for Srf02 to respond */

    if ((millis() - startTime_ms) > timeout_ms)
      return 1;
  }

  out = Wire.read();
  return 0;
}
