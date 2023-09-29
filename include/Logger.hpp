#pragma once

#include <Arduino.h>
#include <initializer_list>
#include <types/Message.hpp>

enum LogLevel { failure, error, warning, info, statistics, transition, debug };

class Logger {
public:
  Logger();
  ~Logger() = default;

  Logger(Logger &&) = delete;
  Logger(const Logger &) = delete;
  Logger &operator=(Logger &&) = delete;
  Logger &operator=(const Logger &) = delete;

  void log(LogLevel, const char *, const char *endStr = "\n");

  /**
   * Log out succession of " " separated values with arbitrary types.
   *
   * WARNING: Attempting to log values not supported by Arduino's Serial.print
   * function will result in compilation errors.
   */
  template <typename... Printable> void log(LogLevel level, Printable... msgs) {
    printLabel(level);

    print(msgs...);

    Serial.print('\n');
  }

  void log(LogLevel, const char *, Message);
  void printLegend();

private:
  void printLabel(LogLevel level);

  template <typename T> void print(T t) {
    Serial.print(t);
    Serial.print(" ");
  }

  template <typename T, typename... Printable> void print(T t, Printable... p) {
    Serial.print(t);
    Serial.print(" ");

    print(p...);
  }
};

extern Logger serial;
