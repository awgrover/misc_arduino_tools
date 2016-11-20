#include <Arduino.h>

#define debugm(msg) Serial.print(msg)

struct TrianglePulse {
  TrianglePulse(
      byte t_min, byte t_max, // range of the pulse
      unsigned long wavelength_ms, // in msec
      float rise_fraction = 0.5 // .25 means rise for 1/4 the wavelength time, fall for 3/4
      ) : t_min(t_min), t_max(t_max)
    {
    value = t_min;
    // calculate it
    float rise_duration = rise_fraction * wavelength_ms;
    calc_deltas(rise_duration, (t_max-t_min), &delta_rise, &delta_rise_delay);
    calc_deltas(wavelength_ms - rise_duration, (t_max-t_min), &delta_fall, &delta_fall_delay);
    delta_fall = -delta_fall;
    delta = delta_rise;
    next_step_at = millis() + delta_rise_delay;
    delta_delay = delta_rise_delay;
    debugm("Tinit for length ");debugm(wavelength_ms);
    debugm(" rise for ");debugm(rise_duration);
    debugm(" rise by ");debugm(delta_rise);debugm(" every ");debugm(delta_rise_delay);
    debugm(" fall by ");debugm(delta_fall);debugm(" every ");debugm(delta_fall_delay);
    debugm("\n");
    }

  void calc_deltas(unsigned long duration, byte range, float *delta, unsigned long *delta_delay) {
    if (duration > range) {
      // msec per delta
      *delta = 1;
      *delta_delay = duration / range; // rounding will lead to drift! 
      }
    else {
      // delta per msec
      *delta_delay = 1;
      *delta = (float) range / duration; // rounding may jump at min/max
      }
    }

  byte t_min, t_max;
  unsigned long delta_rise_delay, delta_fall_delay;
  unsigned long delta_delay;
  float delta_rise, delta_fall;
  float delta;

  float value;

  unsigned long next_step_at; // FIXME: rollover of millis()

  boolean next() { // returns false at end of pulse, but you can keep going for another cycle
    if (millis() >= next_step_at) {
      debugm("its time");debugm(millis());debugm("\n");
      // FIXME: do drift prevention
      value += delta;
      if (delta < 0 && value <= t_min) {
        value = t_min;
        delta = delta_rise;
        delta_delay = delta_rise_delay;
        debugm("  v=");debugm(value);
        }
      else if (delta > 0 && value >= t_max) {
        value = t_max;
        delta = delta_fall;
        delta_delay = delta_fall_delay;
        debugm("  v=");debugm(value);
        }
      next_step_at = millis() + delta_delay;
      debugm(" nxt ");debugm(next_step_at);debugm("\n");
      }
    return value != t_min;
    }
  };
