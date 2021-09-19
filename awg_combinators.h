#pragma once

#include <awg_combinators/value_iface.h>

#include <awg_combinators/serial.h>
#include <Streaming.h> // Streaming.h - supporting the << streaming operator, Mikal Hart

#include <awg_combinators/managed_pins.h>
#include <awg_combinators/ExponentialSmoother.h>

#include <awg_combinators/debounce.h>

// probably factor into "fixes":
template <class I, class O>
O map(I x, I in_min, I in_max, O out_min, O out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// special case for int and char, to prevent "overflow" math
template <class I>
int map(I x, I in_min, I in_max, int out_min, int out_max){
  return (long)(x - in_min) * (long)(out_max - out_min) / (long)(in_max - in_min) + out_min;
}
template <class I>
int map(I x, I in_min, I in_max, byte out_min, byte out_max){
  return (int)(x - in_min) * (int)(out_max - out_min) / (int)(in_max - in_min) + out_min;
}
