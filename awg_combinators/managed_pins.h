#pragma once

// #define NDEBUG to disable assertions
#include <assert.h>

/*
  Makes pin have .value(), () as value, and/or = to set
  Sets the pinmode at init time, no setup() time needed.
*/
class ManagedPin : public ValueSource { // virtual
  public:
  static unsigned long used_pin_mask; // = 0. bit mask of used pins
  const int pin = -1;
  
  ManagedPin(int pin) : pin(pin) {}

  void reserve(int pin) {
    // sadly, we can't say the "pin" in this message
    assert( (used_pin_mask & (1ul << pin)) == 0); // "Pin already in use";
    used_pin_mask |= (1ul << pin);
  }

  boolean is_reserved(int pin) {
    return used_pin_mask & (1ul << pin);
  }

  void release() {
    // harmless to release a pin if it wasn't in use
    used_pin_mask &= ~(1ul << pin);
    }
};

unsigned long ManagedPin::used_pin_mask = 0;

class AnalogPin : public ManagedPin {
  // Input, can't set output
  public:
  AnalogPin(int pin) : ManagedPin(pin) {
    reserve(pin);
    pinMode(pin, INPUT);
    analogWrite(pin, 0);
    }

  int value() { return analogRead( pin ); }
};

class DigitalPin : public ManagedPin {
  public:
  DigitalPin(int pin, int mode=INPUT) : ManagedPin(pin) {
    reserve(pin);
    pinMode(pin, mode);
    }

  int value() { return digitalRead( pin ); }
  void operator=(int hilo) { digitalWrite(pin, hilo); }
  void operator=(boolean hilo) { digitalWrite(pin, hilo); }
};

class PWMPin : public ManagedPin {
  // Output, can't read anything
  public:
  PWMPin(int pin) : ManagedPin(pin) {
    reserve(pin);
    pinMode(pin, INPUT);
    analogWrite(pin,0);
    }

  int value() { return -1; } // doesn't exist yet
  void operator=(int hilo) { analogWrite(pin, hilo); }
  void operator=(boolean hilo) { analogWrite(pin, hilo); }
};
