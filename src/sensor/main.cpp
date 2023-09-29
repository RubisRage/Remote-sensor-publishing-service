#include "Arduino.h"
#include "Srf02.hpp"
#include "sender.hpp"
#include <Logger.hpp>
#include <algorithm>

Srf02 sensor(0xE0, 1);

void setup() {
  Serial.begin(115200);

  while (!Serial)
    ;

  if (sensor.begin() != Srf02::Status::ok) {
    Serial.println("Error initializing sensor");
    while (1)
      ;
  }

  sensor.unit(Srf02::Unit::cm);
}

void loop() {

  Srf02::Status ret = sensor.startMeasurement();

  if (ret == Srf02::Status::ok) {
    uint16_t range;
    sensor.readRange(range);

    sender.send(range);
  }
}
