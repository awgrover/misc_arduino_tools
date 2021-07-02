#pragma once

// Common interface
class ValueSource { // virtual
  public:
  virtual int value() = 0;

  int operator()() { return value(); }
  };
