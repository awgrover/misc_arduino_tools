#include <Arduino.h>

// #define debugm(msg) Serial.print(msg)
#define debugm(msg)

struct LinearSamples {
  LinearSamples(
      byte t_min, byte t_max, // range of the pulse
      unsigned long wavelength_ms // in msec
      ) : t_min(t_min), t_max(t_max)
    {
    value = t_min;
    // calculate it
    calc_deltas(wavelength_ms, (t_max-t_min), &delta_rise, &delta_rise_delay);
    delta = delta_rise;
    next_step_at = millis() + delta_rise_delay;
    delta_delay = delta_rise_delay;
    debugm("Tinit for length ");debugm(wavelength_ms);
    debugm(" rise by ");debugm(delta_rise);debugm(" every ");debugm(delta_rise_delay);
    debugm("\n");
    }

  void calc_deltas(unsigned long duration, int range, float *delta, unsigned long *delta_delay) {
    debugm("calc duration ");debugm(duration);debugm(" ranging ");debugm(range);debugm("\n");
    if (duration > abs(range)) {
      // msec per delta
      *delta = signbit(range) ? -1 : 1;
      *delta_delay = duration / abs(range); // rounding will lead to drift! 
      }
    else {
      // delta per msec
      *delta_delay = 1;
      *delta = (float) range / duration; // rounding may jump at min/max
      }
    }

  byte t_min, t_max;
  unsigned long delta_rise_delay;
  unsigned long delta_delay;
  float delta_rise;
  float delta;

  float value;

  unsigned long next_step_at; // FIXME: rollover of millis()

  boolean next() { // returns false at end of pulse, but you can keep going for another cycle
    if (millis() >= next_step_at) {
      next_step_at = millis() + delta_delay;
      debugm(" nxt ");debugm(next_step_at);debugm("\n");
      debugm("its time");debugm(millis());debugm("\n");
      // FIXME: do drift prevention
      value += delta;
      if (delta < 0 && value < t_max) {
        value = t_min;
        debugm("  v=");debugm(value);
        return false;
        }
      else if (delta > 0 && value > t_max) {
        value = t_min;
        debugm("  v=");debugm(value);
        return false;
        }
      }
    return true;
    }
  };
