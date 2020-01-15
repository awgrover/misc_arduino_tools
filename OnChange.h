#pragma once
/*

# Indicate when a digital signal (HIGH/LOW or true/false) changes. Edge Detection, One-Shot

## Digital Signal Change

Sometimes you want to react only to a change. For example, when a button is pressed, start a song playing or a servo moving (then stop when the button is released). So, do something like this:

  include <debounce.h>
  include "OnChange.h"

  const int switch_pin = 13;
  Debounce switch_debounce(10); // switches are noisy, 10ms debounce
  OnChange switch_change;

  void setup() {
    somemusic.do_more_setup...
    pinMode( switch_pin, INPUT_PULLUP);
  }

  void loop() {
    int raw_switch = digitalRead( switch_pin );
    int debounced = switch_debounce( raw_switch ); // switches are noisy
    int changed = switch_change.changed( debounced );

    if ( changed ) {
      if (debounced == HIGH) {
        somemusic.start_playing(); // and we continue without waiting
      }
      else {
        somemusic.stop();
      }
    }
  }
*/

class OnChange {
  int last;

  public:
  OnChange(int initial) : last(initial) {} // can take an initial state
  OnChange() : last(0) {} // assume 0 initial

  boolean changed(int new_value) {
    if (new_value != last) {
      last = new_value;
      return true;
      }
    else {
      return false;
      }
  }
};
