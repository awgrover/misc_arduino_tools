#include "state_machine.h"

// This example is going to bob & weave, but respond to button presses

// Describe the process: states & transitions.
// A series of STATE blocks

// Bracket the description of one state with STATE and END_STATE
// This says, When we are doing "my_setup", eventually it will be done and go to "bob".
STATE(my_setup) // "my_setup" is a function that gets something done
    WHEN_DONE(bob)  // when it finishes, go to "bob"
END_STATE

STATE(bob)
  // While we are doing "bob", if the button goes down, immediately go to "beep"
  // i.e. interrupt bob'ing
  INTERRUPT_WHEN(momentary_button_is_down, beep_and_go) // while doing "bob", if we notice the button down, go to "my_beep"
  // if "bob" finishes, go to "weave"
  WHEN_DONE(weave) // WHEN_DONE must be the last thing
END_STATE

STATE(weave)
  WHEN_DONE(rest_a_bit)
  INTERRUPT_WHEN(momentary_button_is_down, sit_and_beep)
END_STATE

STATE(rest_a_bit) // they don't have to be in any particular order
  WHEN_DONE(bob) // start over
END_STATE

STATE(sit_and_beep)
  WHEN_DONE(bob) // if we are interrupted beeping, beep and go to bob
END_STATE

STATE(beep_and_go)
  WHEN_DONE(bob) // go back to bob
END_STATE

// the runner is an object that keeps track of which state it is in
// sadly you have to use STATE_NAME()
StateMachine bob_and_weave( STATE_NAME(my_setup) );

boolean my_setup(StateMachine& sm, StateMachinePhase phase) { 
  if (phase == SM_Start) {  } // do the initial stuff
  else if (phase == SM_Running) { } // keep doing stuff
  else if (phase == SM_Finish) { } // clean up
}
boolean bob(StateMachine& sm, StateMachinePhase phase) {} // bobbing
boolean beep_and_go(StateMachine& sm, StateMachinePhase phase) {} // do something
boolean weave(StateMachine& sm, StateMachinePhase phase) {} // do something
boolean sit_and_beep(StateMachine& sm, StateMachinePhase phase) {
  // stop moving
  // run a beep for 2 seconds
  }
boolean rest_a_bit(StateMachine& sm, StateMachinePhase phase) {
  // wouldn't it be nice if I had a patter for this?
  }

boolean momentary_button_is_down() { 
  return digitalRead(9) == LOW; 
  }
/*
*/
