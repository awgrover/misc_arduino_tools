
#include <awg_combinators.h>

template <class I, class O>

StartSerial(115200);

AnalogPinWithDelay a(A0);
ExponentialSmoother smoothed = ExponentialSmoother(&a, 5);
AnalogPinWithDelay b(A1);

void setup() {
  Serial << "Start\n";
  //pinMode(a.pin, INPUT);

}

void loop() {
  //analogRead(a.pin); delay(1);
  int raw = 0; // analogRead(a.pin);
  int value = a.value();
  float blah = map(value, 0, 1024, 0.0, 1.0);
  int mapped = map(value, 0, 1024, 0, 255);

  Serial
      << " raw " << raw
      << " comb " << value
      << " fmapped " << blah
      << " mapped " << mapped
      << " smoothed " << smoothed.value()
      << " b " << b.value()
      << endl;
  delay(100);
}
