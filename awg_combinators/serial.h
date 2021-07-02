#pragma once

// why is this missing?
inline Print &operator <<(Print &obj, const __FlashStringHelper* arg) { obj.print(arg); return obj; }

#define StartSerial(baud) volatile _StartSerial  __StartSerial(baud)
class _StartSerial {
  public:
  // A throw away instance that initializes the serial port at init time
  _StartSerial(unsigned long baud) { 
    Serial.begin(baud);
    Serial << F("Init\n");
  }
};
