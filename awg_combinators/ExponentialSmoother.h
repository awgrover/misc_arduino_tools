#pragma once
#include <ExponentialSmooth.h> // the algorithm

/*
Maybe enable later: 

// Works on anything with .value()
template <typename T>
class ExponentialSmoother : public ValueSource {
  // each .value reads the next value from the underlying x.value()
  public:
  T *valuable;
  ExponentialSmooth<int> *smoother; // the algorithm
  ExponentialSmoother(T *valuable, int factor) : valuable(valuable) {
    smoother = new ExponentialSmooth<int>(factor);
  }
  int value() {
    return smoother->average( valuable->value() );
  }

  void operator=(int newvalue) { smoother->reset( newvalue ); }
};

// don't make a new class for a subclass of ValueSource
template <>
class ExponentialSmoother<ValueSource> : public ValueSource {
*/

class ExponentialSmoother : public ValueSource {
  // each .value reads the next value from the underlying x.value()
  public:
  ValueSource *valuable;
  ExponentialSmooth<int> *smoother; // the algorithm
  // maybe add a & variant
  ExponentialSmoother(ValueSource *valuable, int factor) : valuable(valuable) {
    smoother = new ExponentialSmooth<int>(factor);
  }
  int value() {
    return smoother->average( valuable->value() );
  }

  void operator=(int newvalue) { smoother->reset( newvalue ); }
};
