#pragma once

/*
  Instead of a 3 element array, use a struct with named fields.
  Provides:
    Any type: float, int, etc.
    named fields: .red, .green, .blue
    aliased fields: .r, .g, .b

    Analogous for HSV (hue|h, saturation|sat|s, value|val|v)

  You had:
    float RGB_from_hsv[3];
    // and you did things like
    RGB_from_hsv[1] = RGB_from_hsv[1] * GrnScaler / 100.0;
    HSVehtoRGB(someHSV, RGB_from_hsv);

    for (byte led = 0; led < 3; led++) {
        PWM.set(LunitZones[someZone][led], RGB_from_hsv[led]);  // Display color step.
      }

  Equivalent with rgb structure:
    RGB<float> RGB_from_hsv; // an r,g,b variable (floats) 
    HSVehtoRGB(someHSV, RGB_from_hsv); // no change here, BUT inside it changes to .green etc.

    // now it says "green": mechanical '[1]' -> '.green'
    RGB_from_hsv.green = RGB_from_hsv.green * GrnScaler / 100.0;

    // you can still use []
    for (byte led = 0; led < 3; led++) {
      PWM.set(LunitZones[someZone][led], RGB_from_hsv[led]);  // Display color step.
    }

    // alternative (need to change LunitZones):
    PWM.set(LunitZones[someZone].red, RGB_from_hsv.red);  // Display color step.
    PWM.set(LunitZones[someZone].green, RGB_from_hsv.green);  // Display color step.
    PWM.set(LunitZones[someZone].blue, RGB_from_hsv.blue);  // Display color step.

    // alternative (easy change to PWM.set), an rgb set
    PWM.set(LunitZones[someZone], RGB_from_hsv);  // Display color step.

  Patterns:
    RGB<int> rgb = {5,99,12}; // Initialize easily
    for(int i=0; i<3; i++) { rgb[i] = 1; }; // [] works
    rgb = {1,3,4}; // that works: sets red, green, blue!

    Serial.print(rgb.red); // or .green, or .blue
    rgb.red = 5; // assign in the obvious way
    Serial.print(rgb.r); // .r is the same as .red

    other_ttttest( rgb );

    RGB<int> other_rgb = rgb; // that works, copies values
    other_rgb = rgb; // that works as assignent, copies values

    RGB<float> rgb_a[3] = { // array of RGB's, note nested {}
      {1,2,3}, // each initialized
      {55,66,77},
      {4.5, 6.3, 88.8}
      };

    void other_ttttest(RGB<int> &my_rgb) { // use 'const' if you aren't going to change my_rgb!
      // my_rgb is not a copy (drop the & it will copy!)
      my_rgb.red = 1; // changed for the caller too!

      // don't try to return RGB's
      }
    
*/

template <typename T>
struct RGB {
  // 3 Fields, in rgb order
  // aliased to .r,.g.,.b, and [0], [1], [2]
  union { T red; T r; };
  union { T green; T g; };
  union { T blue; T b; };
  T& operator[](int i) { return *(&red+i); } // tricky and mildly dangerous!
  };

// "padding" could make [] not work, check a few
// No guarantee!
static_assert( sizeof(RGB<float>) == sizeof( float[3] ), "[] for RGB type is broken" );
static_assert( sizeof(RGB<int>) == sizeof( int[3] ), "[] for RGB type is broken" );
static_assert( sizeof(RGB<short>) == sizeof( short[3] ), "[] for RGB type is broken" );
static_assert( sizeof(RGB<byte>) == sizeof( byte[3] ), "[] for RGB type is broken" );

template <typename T>
struct HSV {
  // 3 Fields, in hsv order
  // aliased to short names and [0], [1], [2]
  union { T hue; T h; };
  union { T saturation; T sat; T s; };
  union { T value; T val; T v; };
  T& operator[](int i) { return *(&hue+i); } // tricky and mildly dangerous!
  };

#include "rgb_test.h"
