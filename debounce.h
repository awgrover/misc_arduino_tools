/*
# Digital Debounce And Transients

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

  DebounceHigh sleep(100); // "eyes closed" are longer than blinks (so ignore blinks). "open" is immediate

# Debouncing both directions with different debounce delays

Use DebounceAsymmetric:

  // the sensor is noisier when it goes to HIGH
  DebounceAsymmetric debounce_sensor(HIGH, 100, 10); // starts HIGH, 100 debounce for HIGH, 10 for LOW

# Ignore Short Changes, Hold, Ignore Transients

Sometimes you don't want to respond immediately to a change. Maybe wait 5 seconds before turning off the light.
Or, your signal is a bit noisy so you want to ignore short changes (transients). For example, we had people stroking a piezo sensor,
but stroking is not a continous action, nor very consistent. So, we wanted to ignore gaps in the stroking.
This is like debouncing, or smoothing. It will delay responding to a change, however.

  IgnoreHighTransient hold_on(5000); // 5 second "hold" when changing to HIGH

  void loop() {
    if ( hold_on( digitalRead(9) ) ) {
      digitalWrite(13, HIGH); // light on
      }
    else {
      digitalWrite(13, LOW); // light off, but takes 5 seconds to get there
      }
    }

# Decision Guide

Do you have a noisy digital signal? That means the signal is either HIGH or LOW. Or, do you do a conversion to true and false (e.g. "if analogRead() > 60"), which gives a digital signal? And, does it have extra (spurious) changes from HIGH to LOW?

## Debouncing

Is it only noisy when it changes from HIGH to LOW (and LOW to HIGH)? A switch does this, for example.
Or, any sensor or signal that takes a bit to settle down. Then, you need "debouncing".

* Is it like a switch that is noisy when changing in either direction? Use Debounce.
* Is it when changing in either direction, but different durations of noise depending on the direction ? Use DebounceAssymetric.
* Is it only noisy when changing in one direction, but very clean in the other? Use DebounceHigh or DebounceLow.

## Ignore Transients

Do you get random short changes, or is the signal a bit unstable? A motion detector may do something like that, where
you want to ignore short periods of "non-motion" (or, conversely, short periods of motion). This is something like smoothing. Then you need to "ignore transients".

* Do you only need to ignore LOW transients during a HIGH signal? Use IgnoreLowTransient.
* Do you only need to ignore HIGH transients during a LOW signal? Use IgnoreHighTransient.
* Do you only need to ignore all transients? Use IgnoreTransients.

## Delay Changing

Do you want to delay responding to a change? E.g. maybe wait 5 seconds before turning off the light. This is the same as Ignore Transients, just with a larger duration(s).

E.g., wait 5 seconds after the motion stops, then turn off the light.

  IgnoreLow hold_moving(5000);

  void loop() {
    bool is_motion = motion_detector.read();
    if ( hold_moving(is_motion) ) {
      analogWrite(13, HIGH); // turn on (keep on) the light
      }
    else {
      // hold_moving(is_motion) will stay true for 5 seconds after the detector thinks the motion stopped
      analogWrite(13, LOW); // Elvis has left the room
      }
    }

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
  unsigned long debounce_expire; // starts at 0, which means a "which" signal will count immediately the first time.
  bool last;

  public:
  DebounceAssymetric(bool initial, int duration_high, int duration_low) : duration_high(duration_high), duration_low(duration_low), last(initial) {}

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
  public:
  Debounce(int debounce_duration) : DebounceAsymmetric(HIGH, debounce_duration, debounce_duration) {}
  Debounce(bool initial, int debounce_duration) : DebounceAsymmetric(initial, debounce_duration, debounce_duration) {}
  // Debounce(int initial, int debounce_duration) : DebounceAsymmetric(initial, debounce_duration, debounce_duration) {} // HIGH/LOW are ints
  };

class IgnoreTransients {
  const int duration_high;
  const int duration_low;
  unsigned long holding_expire; // starts at 0, which means a "which" signal will count immediately the first time.
  bool last;

  public:
  IgnoreTransients(bool initial, int duration_high, int duration_low) : duration_high(duration_high), duration_low(duration_low), last(initial) {}
  IgnoreTransients(bool initial, int duration) : duration_high(duration), duration_low(duration), last(initial) {}

  // FIXME: not inline?
  bool operator()(bool hilo) const {
    if (hilo != last) {
      
      // deal with a change (otherwise, it's still the same)

      // are we already holding? (we've already seen the start of a possible transient)
      if (holding_expire) {

        // if holding has expired, take change (after holding)
        if (millis() > holding_expire) {
          holding_expire = 0;
          last = hilo;
          }

        // if not expired, ignore signal (because we are holding)

      }

      // transient possibly started
      else {
        holding_expire = millis() + holding_duration;
      }

    }
    else {
      holding = 0; // if it was a transient, we went back to "last". so, discard the timer
    }

    return last;
  }

template...
IgnoreTransientHigh : immediate response to LOW, init LOW
IgnoreTransientLow : immediate response to HIGH, init HIGH
