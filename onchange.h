/*

# Indicate when a digital signal (HIGH/LOW or true/false) changes. Edge Detection, One-Shot

## Digital Signal Change

Sometimes you want to react only to a change. For example, when a button is pressed, start a song playing or a servo moving (then stop when the button is released). So, do something like this:

  include <debounce.h>
  include <onchange.h>

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
    int changed = switch_change( debounced );

    if ( changed ) {
      if (debounced == HIGH) {
        somemusic.start_playing(); // and we continue without waiting
      }
      else {
        somemusic.stop();
      }
    }
  }

## Edge-Detection or On Start: Change in one direction

Sometimes you only want to know if a digital signal changes from HIGH to LOW, and you don't care if it goes LOW to HIGH (or the opposite situation, of course). For example, you want to start something when a button is pushed, so you really only care when the button is "down", and don't care when the button goes "up". This is the same idea as only caring when something starts. This is EdgeDetection.

  **CONST*

  void setup() {
  }

  eoid loop() {
    int debounced = debounce_switch( digitalRead(switch_pin) ); // short hand for raw_switch + debounced
    if ( switch_closed( debounced ) ) {
      mp3_insult.restart_playing(); // it (re)starts, and we continue, and we'll just let it play till end
      }
    // don't care if the button opens
  }
