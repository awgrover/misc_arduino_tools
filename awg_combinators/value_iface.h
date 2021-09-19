#pragma once

// Common interface
class ValueSource { // virtual
  public:
  virtual int value() = 0;
  virtual int raw_value() { return value(); } // overload with valuable.raw_value()

  int operator()() { return value(); }
  };
