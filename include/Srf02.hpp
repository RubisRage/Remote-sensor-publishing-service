#pragma once /* SRF02_H */

#include "Arduino.h"
#include <cstdint>
#include <functional>

/**
 * This class represents an intance of a Srf02 ultrasonic sensor connected via
 * I2C.
 *
 * @note This class takes control of the Wire object, it should not be used
 * outside this class.
 */
class Srf02 {
public:
  Srf02(uint8_t address, uint8_t sensorId);
  ~Srf02();

  /**
   * Definition of success or error return values.
   */
  enum Status : uint8_t {
    ok,                    /* Successful */
    delay_not_acomplished, /* Tried taking measurement before specified delay
                              time */
    already_initialized,   /* Common dependencies already initialized */
    timeout /* Connection timed out when connecting to the srf02 sensor */
  };

  /**
   * Definition of the Srf02 ultrasonic sensor unit selection codes.
   */
  enum Unit : uint8_t { inc = 80, cm = 81, ms = 82 };

  /**
   * Initializes Wire object, necessary to access the I2C bus. This function
   * must be called before any Srf02 instance is ready to be used.
   */
  static Status begin(void);

  inline uint16_t period() const { return period_ms_; };

  /**
   * Returns the I2C address of the sensor.
   *
   * @return Sensor's I2C address.
   */
  inline uint16_t address(void) const { return address_ << 1; };

  /**
   * Take a distance measurement from the sensor. The returned value's unit
   * depends on the previously set unit, defaults to cm.
   *
   * @param[out] range Variable where to store the taken measurement in case of
   * success.
   *
   * @return Success status, either of:
   *         - ok
   *         - delay_not_acomplished
   */
  Status startMeasurement(void);
  Status readRange(uint16_t &range);

  /**
   * Sets the unit in which measurements should be taken.
   *
   * @param[in] unit used to take measurments from calling moment onwards.
   */
  inline void unit(Srf02::Unit unit) { unit_ = unit; }

  /**
   * Gets the unit in which measurements are being taken.
   *
   * @return Unit in which measurements are being taken.
   */
  inline Srf02::Unit unit(void) const { return unit_; }

  /**
   * Delay time between measurements.
   */
  static constexpr uint16_t delay_ms = 66;

private:
  uint16_t address_;                  /* Sensor's I2C address              */
  uint16_t delay_ms_;                 /* Sensor's current delay time       */
  Srf02::Unit unit_;                  /* Sensor's current measurement unit */
  unsigned long last_measurement_ms_; /* Time of last measurement          */
  uint16_t period_ms_; /* Time period between periodic measurement    */

  /* Whether common dependencies have been initialized (I2C) */
  static bool initialized;

private:
  /**
   * Computes time passed since last measurement.
   *
   * @return Time passed since last measurement.
   */
  inline unsigned long sinceLastMeasurement() {
    return millis() - last_measurement_ms_;
  }

  /**
   * Definition of Srf02 register's and their access codes.
   */
  enum Register : uint8_t {
    command_register = 0,
    range_high_byte = 2,
    range_low_byte = 3,
    autotune_minimum_high_byte = 4,
    autotune_minimum_low_byte = 5
  };

  /**
   * Notifies the Srf02 sensor to use the currently set unit to make the next
   * measurment.
   *
   * @note Should be called each time before reading the high and low bytes of
   * the range register.
   */
  void setUnit(void);

  /**
   * Reads one of the Srf02 sensor's registers.
   *
   * @param[in] srf02_register specifying which register to read.
   * @param[out] out variable in which the register is stored.
   *
   * @return 0 if successfull, 1 on timeout.
   */
  int read_register(Register srf02_register, uint8_t &out);
};
