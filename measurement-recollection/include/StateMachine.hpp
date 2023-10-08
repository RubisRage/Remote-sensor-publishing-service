#pragma once

#include "Logger.hpp"

#define TRANSITION(func) func, #func

class StateMachine {

  typedef void (*state)();

private:
  state current_state;
  const char *current_state_name;

public:
  StateMachine(const state initial, const char *current_state_name)
      : current_state(initial), current_state_name(current_state_name) {}

  ~StateMachine() = default;
  StateMachine &operator=(StateMachine &&) = delete;
  StateMachine(StateMachine &&) = delete;
  StateMachine(const StateMachine &) = delete;
  StateMachine &operator=(const StateMachine &) = delete;

  void transition(const state new_state, const char *new_state_name) {
    if (new_state == nullptr) {
      serial.log(LogLevel::failure, "Tried transitioning to nullptr");
      while (true)
        ;
    }

    serial.log(LogLevel::transition, "Transitioning from \"",
               current_state_name, "\" to \"", new_state_name, "\"");

    current_state = new_state;
    current_state_name = new_state_name;
  };

  const state getState() { return current_state; }
};
