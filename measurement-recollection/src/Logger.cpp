#include <Arduino.h>
#include <Logger.hpp>
#include <algorithm>
#include <array>
#include <string>

struct Level {
  const char *label;
  const char *legend;
};

static std::array<Level, 7> levels = {
    Level{"(FF)", "Complete failure"}, /* LogLevel::failure */
    Level{"(EE)", "Error"},            /* LogLevel::error */
    Level{"(WW)", "Warning"},          /* LogLevel::warning */
    Level{"(II)", "Information"},      /* LogLevel::information */
    Level{"(SS)", "Statistics"},       /* LogLevel::statistics */
    Level{"(TT)", "Transition"},       /* LogLevel::transition */
    Level{"(DD)", "Debug"}             /* LogLevel::debug */
};

Logger::Logger() {}

void Logger::printLabel(LogLevel level) {

  if (level < levels.size()) {
    Serial.print(levels[level].label);
  } else {
    Serial.print("[Unexpected LogLevel]");
  }

  Serial.print(" -> ");
}

void Logger::log(LogLevel level, const char *logMessage, const char *end) {
  printLabel(level);
  Serial.print(logMessage);
  Serial.print(end);
}

void Logger::log(LogLevel level, const char *logMessage, Message message) {
  log(level, logMessage);

  Serial.println("{");
  Serial.print("\t\tSender address: 0x");
  Serial.println(message.sourceAddress, HEX);
  Serial.print("\t\tRecipient address: 0x");
  Serial.println(message.destinationAddress, HEX);
  Serial.print("\t\tMessage ID: 0x");
  Serial.println(message.seq, HEX);
  Serial.print("\t\tMessage type: ");
  Serial.println(message.type < messageTypesNames.size()
                     ? messageTypesNames[message.type]
                     : "[Unexpected message type]");
  Serial.print("\t\tPayload length: ");
  Serial.println(message.payloadLength);
  Serial.print("\t\tPayload: ");

  for (int i = 0; i < message.payloadLength; i++) {
    Serial.print(message.payload[i], HEX);
    Serial.print(" ");
  }

  Serial.println("\n}");
}

void Logger::printLegend() {
  Serial.println("Log legend\n");

  for (size_t i = 0; i < sizeof(levels) / sizeof(Level); i++) {
    Serial.print("\t\t");
    Serial.print(levels[i].label);
    Serial.print(": ");
    Serial.println(levels[i].legend);
  }
}

/* Global declarations (extern) */
Logger serial;
