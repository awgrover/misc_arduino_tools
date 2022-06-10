/*
  works on samd
*/

#if defined(NEOPIXEL_NUM) && NEOPIXEL_NUM > 0
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel neopixels(NEOPIXEL_NUM, NEOPIXEL_NUM);
#define clear_bright_leds() neopixels.begin();neopixels.clear();
#elif defined(DOTSTAR_NUM) && DOTSTAR_NUM > 0
#include <SPI.h>
#include <Adafruit_DotStar.h>
Adafruit_DotStar strip(DOTSTAR_NUM, PIN_DOTSTAR_DATA, PIN_DOTSTAR_CLK);
#define clear_bright_leds()  strip.begin();strip.clear();strip.show();
#else
#define clear_bright_leds()
#endif

void setup() {
  clear_bright_leds();
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(115200);

  static unsigned int serial_start = millis();
  while ( ! Serial && (millis() - serial_start < 1000) ) {
    delay(10);
  }
 
  // Info
  Serial.print(F( "Start " __FILE__ " " __DATE__ " " __TIME__ " gcc " __VERSION__ " ide "));
  Serial.println(ARDUINO); // ide version
  // Nope, can't get architecture!

  Serial.print("Digital pins 0.."); Serial.println(NUM_DIGITAL_PINS - 1);
  Serial.print("Analog pins A0..A"); Serial.println(NUM_ANALOG_INPUTS - 1);
  for (unsigned int i = 0; i < NUM_DIGITAL_PINS; i++) {
    int analog_pin = -1;

    // is this an analog pin?
    // analogInputToDigitalPin() may not be a continous range, so search
    for (unsigned int a_i = 0; a_i < NUM_ANALOG_INPUTS; a_i++) {
      if ( analogInputToDigitalPin(a_i) == i ) {
        analog_pin = a_i;
        break;
      }
    }

    if ( analog_pin != -1) {
      Serial.print("A"); Serial.print(analog_pin); Serial.print(" ");
    }
    Serial.print(i);


    if (digitalPinHasPWM(i)) {
      Serial.print(" ");
      Serial.print("pwm");
    }
    Serial.println();
  }
}


void loop() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500); // allow native usb etc
}
