#pragma once

#include <every.h>

class Debounce : public ValueSource {
  // simple debounce for a switch or button
  // if you just want to know it's state
  public:
  ValueSource *valuable;
  Timer ignoring = Timer(100);
  int last_value;

  Debounce( ValueSource *valuable, int debounce_period=100) : valuable(valuable) {
    ignoring.reset(debounce_period);
    ignoring.running = false; // we start ready to read a value
    }

  int value() {

    if ( ignoring.after() ) {
      int next_value = valuable->value();
      if ( last_value != next_value ) {
        last_value = next_value;
        ignoring.reset();
        return last_value;
        }
      }

    return last_value;
    }
  
};

