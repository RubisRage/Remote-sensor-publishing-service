#include "range.hpp"
#include "Logger.hpp"
#include "Srf02.hpp"
#include <Timer.hpp>
#include <cstdint>

Buffer<uint16_t, range_buffer_capacity> range_buffer;
static Srf02 sensor(sensor_address);
static Timer range_timer(sensor_read_period);

void init_sensor() {
  if (sensor.begin() != Srf02::Status::ok) {
    serial.log(LogLevel::failure, "Sensor init failed. Aborting...");
    while (1)
      ;
  }

  sensor.unit(sensor_unit);
}

void store_range() {

  if (!range_timer.hasTimedOut(false)) {
    return;
  }

  sensor.startMeasurement();

  uint16_t range;
  Srf02::Status ret = sensor.readRange(range);

  switch (ret) {
  case Srf02::Status::delay_not_acomplished:
    break;

  case Srf02::Status::ok:
    if (!range_buffer.push(range)) {
      serial.log(LogLevel::error,
                 "Range buffer overflow. Dropping measurement.");
    }
  case Srf02::Status::timeout:
    range_timer.reset();
    break;

  default:
    serial.log(LogLevel::warning,
               "Unexpected sensor status for Srf02::readRange.");
  }
}
