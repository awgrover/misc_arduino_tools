/*
# Debounce

If you have a noisy signal, like a switch, you often want to "debounce". That means, ignore rapid changes (noise) in 
a signal after it changes.
e.g. a switch is often configured as HIGH when open, and LOW when closed/pressed. The signal would look like:
  on close: (use to be HIGH), LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW...
  on open: (use to be LOW), HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH, HIGH, ...
You usually don't want to respond to those alternations.

So, you could do this:

  const int switch_pin = 9; // switch on pin 9
  Debounce debounce_switch(10); // 10 ms debounce

  void setup() {
    pinMode(switch_pin, INPUT_PULLUP); // unpressed is HIGH
    }
    
  void loop() {
    bool pressed;
    
    // debounce the raw signal
    pressed = debounce_switch( digitalRead(switch_pin) );

    if (pressed) {
      Serial.println('P');
    }
    else {
      Serial.println('-');
    }
  }

# Debouncing just one direction

Use DebouceHigh or DebounceLow.

  DebounceHigh wink(100); // winks are longer than blinks (so ignore blinks). "unwink" is immediate

# Debouncing both directions with different debounce delays

Use DebounceAsymmetric:

  // this sensor is noisier when it goes to HIGH
  DebounceAsymmetric debounce_sensor(HIGH, 100, 10); // starts HIGH, 100 debounce for HIGH, 10 for LOW

# Ignore Short Changes, Hold, Ignore Transients

Sometimes you don't want to respond immediately to a change. Maybe wait 5 seconds before turning off the light.
Or, your signal is a bit noisy so you want to ignore short changes (transients). For example, we had people stroking a piezo sensor,
but stroking is not a continous action, nor very consistent. So, we wanted to ignore gaps in the stroking.
This is like debouncing, or smoothing. It will delay responding to a change, however.

  DebounceHigh hold_on(5000); // 5 second "debounce" when changing to HIGH

  void loop() {
    if ( hold_on( digitalRead(9) ) ) {
      digitalWrite(13, HIGH); // light on
      }
    else {
      digitalWrite(13, LOW); // light off, but takes 5 seconds to get there
      }
    }

# Decision Guide

Do you have a noisy digital signal? That means the signal is either HIGH or LOW. Or, do you do a conversion to true and false (e.g. "if analogRead() > 60"). And, does it have extra (spurious) changes from HIGH to LOW?

Is it only noisy when it changes from HIGH to LOW (and LOW to HIGH)? A switch does this, for example.
Or, any sensor or signal that takes a bit to settle down. Then, you need "debouncing".

## Debouncing

* Is it like a switch that is noisy when changing in either direction? Use Debounce.
* Is it when changing in either direction, but different durations of noise depending on the direction ? Use DebounceAssymetric.
* Is it only noisy when changing in one direction, but very clean in the other? Use DebounceHigh or DebounceLow.

Do you get random short changes, or is the signal a bit unstable? A motion detector may do something like that, where
you want to ignore short periods of "non-motion" (or, conversely, short periods of motion). This is something like smoothing. Then you need to "ignore transients".

## Ignore Transients

* Do you only need to ignore LOW transients during a HIGH signal? Use IgnoreLow.
* Do you only need to ignore HIGH transients during a LOW signal? Use IgnoreHigh.
* Do you only need to ignore all transients? Use IgnoreTransients.

*/

template<bool which>
class DebounceWhich {
  const int duration;
  unsigned long debounce_expire; // starts at 0, which means a "which" signal will count immediately the first time.
  bool last;

  public:
  DebounceHigh(int duration) : duration(duration), last(!which) {}

  // FIXME: don't inline
  bool operator()(bool hilo) const {
    if (hilo != last) {

      // signal changed, deal with it

      // ignore changes during the debounce period
      if (millis() > debounce_expire) {
        // if we changed towards the noisy signal, "which", then start a debounce_period
        if (hilo == which) {
          debounce_expire = millis() + duration; // ignore till expired 
        }
        last = hilo; // we immediately take the change (we just ignore noise after it for a while)
      }

    return last; // always return the debounced value
  }

};

using DebounceHigh = DebounceWhich<true>;
using DebounceLow = DebounceWhich<false>;

class DebounceAsymmetric {
  // debounce both directions, different debounce

  const int duration_high;
  const int duration_low;
  unsigned long debounce_high_expire; // starts at 0, which means a "which" signal will count immediately the first time.
  unsigned long debounce_low_expire; // starts at 0, which means a "which" signal will count immediately the first time.
  bool last;

  public:
  DebounceHigh(bool initial, int duration_high, int duration_low) : duration_high(duration_high), duration_low(duration_low), last(initial) {}

  // FIXME: don't inline?
  bool operator()(bool hilo) const {
    if (hilo != last) {

      // signal changed, deal with it

      // ignore changes during the debounce period
      if (millis() > debounce_expire) {
        // if we changed, then start a debounce_period
        debounce_expire = millis() + (hilo ? duration_high : duration_low); // ignore till expired 
        last = hilo; // we immediately take the change (we just ignore noise after it for a while)
      }

    return last; // always return the debounced value
    }

  }

}

class Debounce : DebounceAsymmetric {
  // FIXME: we could use 1 less int by rewriting the asymmetric class
  Debounce(int debounce_duration) : DebounceAsymmetric(HIGH, debounce_duration, debounce_duration) {}
  Debounce(bool initial, int debounce_duration) : DebounceAsymmetric(initial, debounce_duration, debounce_duration) {}
  // Debounce(int initial, int debounce_duration) : DebounceAsymmetric(initial, debounce_duration, debounce_duration) {} // HIGH/LOW are ints
  };
