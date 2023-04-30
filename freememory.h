#pragma once

// freeMemory() should give bytes of heap that are free
//
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#elif defined(ESP32)
#include <Esp.h>
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {

#ifdef __arm__
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(ESP32)
  return ESP.getFreeHeap();
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  char top;
  return &top - __brkval;
#else  // __arm__
  char top;
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
