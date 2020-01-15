#pragma once

#include "ExponentialSmooth.h"
#include "CrossOverDetect.h"

class CapTouchCrossover {
  /* 
  If you have a signal (e.g. a typical UNO analogRead used as a cap-touch),
  which does not auto-calibrate well,
  this might work:

  It uses an exponential-smooth to follow the signal, taking some noise out,
  and a second, "slower" exponential-smooth as the comparison.

  When the 2 cross (because the slower lags behind), we call it a "change": i.e. a touch/release.

  This seems to work pretty well for the uno-analogRead/captouch (after
  tuning resistor values, etc.).

  You still have to provide some tuning parameters:
    the exponential-smooth parameter (analogous to sample-window),
    a delta that has to be exceeded to count as cross-over (effectively debounces)
  For different environments, you have to adjust those. E.g. on a table, on a second table.

  Usage:
    
    CapTouchCrossover touch1(A0, 20, 50, 10);
    void setup() {
      touch1.setup();
      }

    void loop() {
      ...
      touch1.read(); // does the anlogRead(), slows things down. could do periodically
      if (touch1.touched()) { do something for touched }
      else if (touch1.released()) { do something for released }
      ...

      // also available are all the CrossOverDetect methods on .crossover

      // test on plotter (comment out when not testing):
      touch1.debug_print();
      }

  
  Todo: a training mode that tries to derive the parameters (at least delta).
  */

  private:

  public:
  // leave attributes public
  int pin;
  CrossOverDetect<ExponentialSmooth> &crossover; // .v1.value

  // Intended for the analogRead(), so works in the int domain
  CapTouchCrossover(
    int analog_pin, // which analog pin, e.g. A0 
    int fast, // sample-width for the fast follower. large enough to remove most noise, but fast enough to be responsive
    int slow, // .. for the slow follower. larger than fast, to give the delta at crossover, but small enough to allow rapid release
    int delta
    ) : pin(analog_pin),
      // start assumes non-touching
      crossover( *(new CrossOverDetect<ExponentialSmooth>(delta, new ExponentialSmooth(slow), new ExponentialSmooth(fast), -1)) )
    {}

  void setup() {
    // at void setup()
    // so we start at a current read
    crossover.v1.reset( crossover.v2.reset(analogRead(pin)) ); // initial value
  }

  int read() {
    int v = analogRead(pin) - analogRead(A5);
    crossover.v1.average( v );
    crossover.v2.average( v );
    return v; // convenience
  }

  boolean touched() {
    // true only once, till released and touched again
    return crossover.d_on();
  }

  boolean released() {
    // true only once, till touched and released again
    return crossover.d_off();
  }

  boolean touching() {
    return crossover.on();
  }

  boolean not_touching() {
    return crossover.off();
  }

  void debug_print() {
    // suitable for plotting, to see the values for tuning the params
    // NB: you supply trailing Serial.println() !
    // NB: Do your Serial.begin()!
    Serial.print( touching() * 10 + not_touching() * -10 + 1);Serial.print(" "); // x 10 so you can see it in a plot, +1 vs 0 for indeterminate
    Serial.print( crossover.v1.value() - 50 );Serial.print(" ");
    Serial.print( crossover.v2.value() - 50 );Serial.print(" ");
    Serial.print( crossover.v1.value() - crossover.v2.value() );Serial.print(" "); // the measured delta
  }

};
