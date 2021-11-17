
#include <awg_combinators.h>

StartSerial(115200);


Debounce a_switch(new Switch(6));

void setup() {
  Serial << "Start\n";

}

void loop() {

  DigitalPin* dp = (DigitalPin*) a_switch.valuable;
  Serial
      << " pin " << dp->pin
      << " raw " << digitalRead(dp->pin)
      << " sw " << a_switch.value()
      << endl;
  delay(100);
}
