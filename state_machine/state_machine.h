// A non-blocking state machine
// run each state till it says done, then go to the next state; or detect an event and go to another state
// Allows you to specify a process and detect events to change what you are doing

/* Usage

  #include "state_machine.h"

  // This example is going to bob & weave, but respond to button presses
  // Describe the process: states & transitions.
  // A series of STATE blocks
  // The order of the STATE blocks isn't supposed to matter
  // Write the state functions above here as you go

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
  StateMachine bob_and_weave( STATE_NAME(my_setup) ); // after the STATE() declarations above

  void setup() {
    bob_and_weave.run(); // do the first state, i.e. setup, if you feel it necessary to separate it out
    }

  void loop() {
    bob_and_weave.run();
    }

  // You should be able to write the STATE declarations above the state-functions

  boolean my_setup(StateMachine& sm, StateMachinePhase phase) { 
    if (phase == SM_Start) {  } // do the initial stuff
    else if (phase == SM_Running) { } // keep doing stuff
    else if (phase == SM_Finish) { } // clean up
  }
  boolean bob(StateMachine& sm, StateMachinePhase phase) {
    // bobbing
    const int ct;
    if (phase == SM_Start) { ct=2; return true;} // setup
    else if (phase == SM_Running) { 
      Serial.println("bobbing..."); 
      ct = ct -1;  // count down
      return ct > 0;
      }
    else if (phase == SM_Finish) { return false; } // we don't need to cleanup
  }

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

*/
/* Common errors

error: '_beep_and_go_xtion' was not declared in this scope
             return & _ ## next_state ##_xtion; \
  # Note the pattern of the second line. 
  # Note the extra stuff around "beep_and_go"
  # You haven't made a STATE(beep_and_go), but you referred to it.
  # OR, there is some other typo messing everything up
  # So describe it.

error: 'my_setup' was not declared in this scope
     if ( statefn(sm, phase) ) {
  # Note the pattern of the second line
  # You have "my_setup" in a STATE() or WHEN_DONE(), but you didn't write the function for it later in the file. 
  # E.g, make something like "my_setup" above

error: too few arguments to function 'boolean momentary_button_is_down(StateMachine&, StateMachinePhase)'
         if ( pred() ) { \
  # Note the second line.
  # You miswrote the function that appears first in an INTERRUPT_WHEN(). Hint, the first is a test.
  # Or your have the arguments to INTERRUPT_WHEN() reversed.
  # It should looke like : boolean momentary_button_is_down() { .... return true or false }

error: too many arguments to function 'boolean rest_a_bit()'
     if ( statefn(sm, phase) ) {
  # Note the second line.
  # You miswrote the function that appears second in an INTERRUPT_WHEN(). Hint, the second is a state function
  # Or your have the arguments to INTERRUPT_WHEN() reversed.
  # It should looke like : boolean rest_a_bit((StateMachine& sm, StateMachinePhase phase)) { ... }

/* Examples
*/
/* Details

To signal that a machine wants to stop, arrange for the next state to be NULL ( e.g. WHEN_DONE(NULL) )
your_machine.run() will return false

Predicates can be template functions or functions.

States can only be the "name" of a state function.

"Phase"

Errors are not dealt with. Maybe add a flag to the struct, and test in the run().

Really want to be able to do my "wait_for<20>" tricks IN the state engine. Maybe with a different macro.

Reuse a state machine: can't exit with null, but needs to exit. leave off the final state?

Delays,
  auto delay30 = sm_delay<30>;
  STATE(delay30) WHEN_DONE(nextstate) END_STATE

A heartbeat looks like this:
  INTERRUPT_WHEN(the_heartbeat, act)

  boolean the_heartbeat() {
      // example timer heartbeat, there is a everymillis<5000>, and nthTime<1000>
      static unsigned long every = millis() + 5000;
      Serial.print(every-millis());Serial.print(" ");
      if (millis() > every) {
          every = millis() + 5000;
          return true;
          }
      return false;
      }

*/
/*
future design notes

1. STATE(from, to). gets rid of WHEN_DONE. can use NULL for to.
2. GOTO_WHEN(pred, to) instead of interrupt?
3. template based
  auto statea = state<action, pred/to.., to>
  now can reuse state functions, etc. but declare order?
*/

#define DEBUG 0
#if DEBUG==1
  #define debugm(msg) Serial.print(msg)
#else
  #define debugm(msg)
#endif

struct StateMachine;
enum StateMachinePhase { SM_Start, SM_Running, SM_Finish };
typedef boolean (*StateFnPtr)(StateMachine& sm, StateMachinePhase phase); // preserve our sanity
typedef boolean (&StateFnRef)(StateMachine& sm, StateMachinePhase phase); // convenience for the initializer & user
// guess who can't do typedefs that reference themselves?
// from http://www.gotw.ca/gotw/057.htm
struct StateXtionFnPtr_; // declare functions returning this, they can get autocoerced to StateXtionFnPtr
typedef StateXtionFnPtr_ (*StateXtionFnPtr)(StateMachine& sm, StateMachinePhase phase); // vars as this
struct StateXtionFnPtr_ {
    StateXtionFnPtr_( StateXtionFnPtr pp ) : p( pp ) { } // auto-coerce a *fn to StateXtionFnPtr_
    operator StateXtionFnPtr() { return p; } // auto-coerce a struct to StateXtionFnPtr
    StateXtionFnPtr p;
    };
// the state of the machine
struct StateMachine {
    // subclass for your own user-data
    boolean changed;
    StateXtionFnPtr current;
    // User data:
    union {
      unsigned long user_long;
      int user_int;
      };

    // we start out "changed" (from null) to startstate
    StateMachine(StateXtionFnPtr startstate) : current(startstate), changed(true) {
      debugm("SM_Start, SM_Running, SM_Finish\n"); debugm(SM_Start);debugm(", "); debugm( SM_Running);debugm(", "); debugm( SM_Finish);debugm("\n");
      }

    boolean run() {
        // someone might try to run us after we signaled "all done"
        debugm("Step FROM ");debugm((long)current);debugm(changed ? " STRT" : " RUN");debugm("...");
        if (current == NULL) {
            return NULL;
            }

        // The StateXtionFn (_realstatename_xtion) returns "what to do next", which may be stay
        StateXtionFnPtr next_state = (*current)(*this, changed ? SM_Start : SM_Running);
        changed = next_state != current;
        if (DEBUG) {
          if (changed) { debugm(" !->");debugm((long)next_state);debugm("\n"); }
          else { debugm(" again\n"); }
          }


        // update
        current = next_state;

        // a null means the state-machine wants to quit
        return current == NULL;
        }
    };

// utility for users for new()
#define STATE_NAME(statefn) _ ## statefn ##_xtion
// you have to DECLARE a state if it is used before STATE declares it. Not true?
// e.g.: error: '_play_message_xtion' was not declared in this scope
// note the "_" and "_xtion"
#define DECLARE_STATE(statefn) StateXtionFnPtr_ _ ## statefn ##_xtion(StateMachine& sm, StateMachinePhase phase);
// we capture the statefn in self so we can use it
// and open the "if running" block
#if DEBUG
  #define STATE(statefn) StateXtionFnPtr_ _ ## statefn ##_xtion(StateMachine& sm, StateMachinePhase phase) { \
      const StateXtionFnPtr self = & _ ## statefn ## _xtion; \
      const StateFnPtr self_state = statefn; \
      debugm("["#statefn"@");debugm((long)statefn);debugm("(");debugm(phase);debugm(")"); \
      boolean rez = statefn(sm, phase); \
      debugm(":");debugm(rez);debugm("]"); \
      if ( rez ) {
#else
  #define STATE(statefn) StateXtionFnPtr_ _ ## statefn ##_xtion(StateMachine& sm, StateMachinePhase phase) { \
      const StateXtionFnPtr self = & _ ## statefn ## _xtion; \
      const StateFnPtr self_state = statefn; \
      if ( statefn(sm, phase) ) {
#endif
// while running, the pred->next can be triggered
// ignore result from calling a state with FINISH
#define INTERRUPT_WHEN(pred, next_state) \
        if ( pred() ) { \
            (*self_state)(sm, SM_Finish); \
            return & _ ## next_state ##_xtion; \
            } \
// when this state is finished: "else"
// close  the if,  return
// if missing, returns NULL
#if DEBUG
  #define WHEN_DONE(next_state) \
      } \
      else { \
          debugm(" finish ");debugm((long)self_state);debugm("\n"); \
          (*self_state)(sm, SM_Finish); \
          return & _ ## next_state ## _xtion;
#else
  #define WHEN_DONE(next_state) \
      } \
      else { \
          (*self_state)(sm, SM_Finish); \
          return & _ ## next_state ## _xtion;
#endif
// the return is dead code if the "else" above get constructed, but it's the final line for the "if" otherwise
// close the if/else
// terminate if finished & nothing specified (no else) 
#if DEBUG
  #define END_STATE return self; \
          } \
      debugm("Fell through to default NULL\n"); \
      return self; \
      }
#else
  #define END_STATE return self; \
          } \
      return self; \
      }
#endif

template<int n>
boolean nthTime() {
  // every nth-time the pred fires, like a heartbeat
  // Has shared state for your entire program (per "n")
  static int ct=n;
  ct = ct-1;
  if (ct) { return false; }
  ct=n;
  // Serial.print(millis());Serial.println(" hit"); 
  return true;
  }

template<int n>
boolean everymillis() {
  // every n millis the pred fires, like a heartbeat
  // Has shared state for your entire program (per "n")
  static unsigned long every = millis()+n;
  if (millis() < every) { return false; }
  every = millis()+n;
  // Serial.print(millis());Serial.println(" hit"); 
  return true;
  }

template<int delayms>
boolean sm_delay(StateMachine& sm, StateMachinePhase phase) {
  static unsigned long till;

  if (phase == SM_Finish) { 
    // Serial.print("expired ");Serial.println(millis());
    return false; 
    }

  if (phase == SM_Start) { till = millis() + delayms; }

  return millis() < till;
  }

template<int pin, int hilo>
boolean sm_digitalWrite(StateMachine& sm, StateMachinePhase phase) {
  if (phase == SM_Finish || phase==SM_Start) { return true; }
  Serial.print(pin);Serial.print("->");Serial.println(hilo);
  digitalWrite(pin,hilo);
  return false;
  }

// Some boolean combinators for predicates, since we can't do an expression
// Must enclose in () in the macros! ugly
// eg.. INTERRUPT_WHEN( (&SM_and<onhook, offhook>), play_message)
typedef boolean (&SimplePredicate)();
template <SimplePredicate a, SimplePredicate b> boolean SM_and() { return a() && b(); }
// for preds that want the state, ?
