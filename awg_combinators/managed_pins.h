#pragma once

// #define NDEBUG to disable assertions
#include <assert.h>

/*
  Makes pin have .value(), () as value, and/or = to set
  Sets the pinmode at init time, no setup() time needed.
*/
class ManagedPin : public ValueSource { // virtual
  // object() is object.value() from ValueSource
  public:
  static uint16_t used_pin_mask; // = 0. bit mask of used pins
  const int pin = -1;
  const int _mode = INPUT;
  
  ManagedPin(int pin, int mode) : pin(pin), _mode(mode) {}

  void reserve(int pin) {
    // sadly, we can't say the "pin" in this message
    assert( (used_pin_mask & (1ul << pin)) == 0); // "Pin already in use";
    used_pin_mask |= (1ul << pin);
  }

  boolean setup() { return true; }; // return false on fail. pinMode() etc

  boolean is_reserved(int pin) {
    return used_pin_mask & (1ul << pin);
  }

  void release() {
    // harmless to release a pin if it wasn't in use
    used_pin_mask &= ~(1ul << pin);
    }
};

uint16_t ManagedPin::used_pin_mask = 0;

class AnalogPin : public ManagedPin {
  // Input, can't set output
  public:
  AnalogPin(int pin) : ManagedPin(pin, INPUT) {
    reserve(pin);
    }

  boolean setup() {
    pinMode(pin, _mode);
    return true;
    }

  int value() { return analogRead( pin ); }
};

class AnalogPinWithDelay : public AnalogPin {
  // prevents ADC interference between pins at the cost of a 1msec blocking delay
  public:
  AnalogPinWithDelay(int pin) : AnalogPin(pin) {}

  int value() { 
    analogRead(pin); delay(1);
    return analogRead( pin );
    }
};

class DigitalPin : public ManagedPin {
  public:
  DigitalPin(int pin, int mode=INPUT) : ManagedPin(pin, mode) {
    reserve(pin);
    }

  boolean setup() {
    pinMode(pin, _mode);
    return true;
    }

  int value() { return digitalRead( pin ); }
  void operator=(int hilo) { digitalWrite(pin, hilo); }
  void operator=(boolean hilo) { digitalWrite(pin, hilo); }
};

class Switch : public DigitalPin {
  // just does INPUT_PULLUP and inverts the value
  public:
  Switch(int pin) : DigitalPin(pin, INPUT_PULLUP) {}
  int value() { return ! DigitalPin::value(); }
};

class PWMPin : public ManagedPin {
  int _value = 0;
  public:
  PWMPin(int pin) : ManagedPin(pin, INPUT) {
    reserve(pin);
    }

  boolean setup() {
    pinMode(pin, _mode);
    analogWrite(pin,0);
    return true;
    }

  int value() { return _value; }
  void operator=(int hilo) { _value = hilo; analogWrite(pin, hilo); }
  void operator=(boolean hilo) { (*this) = hilo ? 255 : 0; }
};
