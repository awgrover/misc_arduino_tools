#pragma once
#include "ExponentialSmooth.h"

class PeakTrack {
  /*
  Track the peak of a signal that "oscillates".
  Doesn't have to be clean/sinusoidal.
  Does abs() on the value, so treats <0 as symmetrical around 0.
  But, also works if offset from  0, just uses the max positive value

  ExponentialSmooth smooth1 = ExponentialSmooth(20);
  PeakTrack osc1(5); // bah.... ### track min/max then derive how to detect peak

  // should use a smoothed value
  smooth1.average( analogRead(A0) ); // update read
  osc1.update( smooth1 );
  
  println( osc1.peak() ); // tracked peak

  */

  private:
  ExponentialSmooth *decay;

  public:
  PeakTrack(int beta) { this->decay = new ExponentialSmooth(beta); } // exp decay of peak

  int update(int new_value) {
    if (new_value > decay->value()) {
      return decay->reset(new_value);
      }
    else {
      return decay->average( 0 );
      }
  }

  int value() { return decay->value(); }
};
