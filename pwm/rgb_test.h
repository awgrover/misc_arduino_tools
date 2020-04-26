/* Compile test, not used in running code */

void other_ttttest(RGB<int> &my_rgb);

void ttttest() {
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
  if( rgb_a) {} // prevent unused warning
  
  }

void other_ttttest(RGB<int> &my_rgb) {
  // my_rgb is not a copy
  my_rgb.red = 1; // changed for the caller too!

  // don't try to return local variable RGB's
  }
