// v2 more flexible

/* Future:
    template< action, holdms>
    SEQUENCE({first, ...., last}, next)
    rewrite for tail-call optimization!
*/
/* debugging errors

    Did you get a lot of errors? A typo somewhere is the problem.
        If this is at the top:
          error: 'somename' was not declared in this scope
            STATE(somename, somthingelse)
        then temporarily put the declaration just above the STATE() and try again. like:
          boolean somename(); // temporary
          STATE(somename, somthingelse)
    Remember to remove the temporary stuff from above
    SIMPLExxx doesn't get END_STATE, can't use GOTOWHEN
    But STATExxx needs END_STATE

    error: macro "STATEAS" passed 4 arguments, but takes just 3
    # if you use one of the sm_xxx<> things, you need parenthesis:
    #   STATEAS(somename, ( sm_digitalWrite<PIN1, HIGH> ), nextsomething)
    # Check for the proper STATE(from,to), STATEAS(name, from, to), etc.

    error: conversion from '<unresolved overloaded function type>' to non-scalar type 'StateXtionFnPtr_' requested
             }; \
    # Did you forget an END_STATE?

    error: '_ring_on_xtion' was not declared in this scope
         return one_step(sm, action_function_wrapper<action>, _##name##_xtion, NOPREDS, _##next_state##_xtion); \
    # Do you have a STATE(ring_on...) ?

*/

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG==1
  #define debugm(msg) Serial.print(msg)
#else
  #define debugm(msg)
#endif

struct StateMachine;
typedef boolean (*ActionFnPtr)(StateMachine& sm); // your action functions

enum StateMachinePhase { SM_Start, SM_Running, SM_Finish };

struct StateXtionFnPtr_; // declare functions returning this, they can get autocoerced to StateXtionFnPtr
typedef StateXtionFnPtr_ (&StateXtionFnRef)(StateMachine& sm);
typedef StateXtionFnPtr_ (*StateXtionFnPtr)(StateMachine& sm); // vars as this
struct StateXtionFnPtr_ {
    StateXtionFnPtr_( StateXtionFnPtr pp ) : p( pp ) { } // auto-coerce a *fn to StateXtionFnPtr_
    operator StateXtionFnPtr() { return p; } // auto-coerce a struct to StateXtionFnPtr
    StateXtionFnPtr p;
    };
// the state of the machine
struct StateMachine {
    // subclass for your own user-data
    StateMachinePhase phase;
    StateXtionFnPtr current;
    // User data:
    union {
      unsigned long user_long;
      int user_int;
      };
    // first state will be SM_Start
    StateMachine(StateXtionFnPtr startstate) : current(startstate), phase(SM_Start) {
      debugm("SM_Start, SM_Running, SM_Finish\n"); debugm(SM_Start);debugm(", "); debugm( SM_Running);debugm(", "); debugm( SM_Finish);debugm("\n");
      }

    void restart( StateXtionFnPtr_  xtion) {
      current = xtion;
      // do we need to call finish here?
      phase = SM_Start;
      }

    boolean run() {
        // debugm("Step action ");debugm((long)current);debugm(" @");debugm(phase);debugm("...");

        // someone might try to run us after we signaled "all done"
        if (current == NULL) {
            return false;
            }

        // The StateXtionFn (_realstatename_xtion) returns "what to do next", which may be stay
        StateXtionFnPtr next_state = (*current)(*this);

        if (next_state != current) { 
            // debugm(" !->");debugm((long)next_state);debugm("\n"); 
            }
        else { 
            phase = SM_Running;
            // debugm(" again\n"); 
            }

        // update
        current = next_state;

        // a null means the state-machine wants to quit
        return current == NULL;
        }

    };

typedef boolean (*BooleanFnPtr)();

template<BooleanFnPtr pred, StateXtionFnRef next_state> StateXtionFnPtr_ gotowhen(StateMachine &sm) { return (*pred)() ? next_state : NULL; }

// Action functions can have a variety of signatures:
// return boolean or not: true means "do me again till I give false". void means "do me once"
// Take any or none of: StateMachine &sm, StateMachinePhase phase
//   If you take the phase, you will see at least 3 calls: SM_Start, SM_Running, SM_Finish. SM_running will repeat till you say false
// FIXME: not handling phase right
template<boolean fn(StateMachine &sm, StateMachinePhase phase)> inline boolean action_function_wrapper(StateMachine &sm) { return fn(sm, sm.phase); }
template<boolean fn(StateMachine &sm)> inline boolean action_function_wrapper(StateMachine &sm) { return sm.phase==SM_Finish ? false : fn(sm); }
template<boolean fn(StateMachinePhase phase)> inline boolean action_function_wrapper(StateMachine &sm) { return fn(sm.phase); }
template<boolean fn()> inline boolean action_function_wrapper(StateMachine &sm) { return sm.phase==SM_Finish ? false : fn(); }
template<void fn(StateMachine &sm, StateMachinePhase phase)> inline boolean action_function_wrapper(StateMachine &sm) { fn(sm, sm.phase); return false; }
template<void fn(StateMachine &sm)> inline boolean action_function_wrapper(StateMachine &sm) { if(sm.phase != SM_Finish) {fn(sm);} return false;}
template<void fn()> inline boolean action_function_wrapper(StateMachine &sm) { if(sm.phase != SM_Finish) {fn();} return false; }

inline void debug_time() { debugm("[");debugm(millis());debugm("] "); }
inline void debug_phase(StateMachine &sm) { 
  #if DEBUG==1
    static const char *phasename[] = { "SM_Start", "SM_Running", "SM_Finish" };
    debugm(phasename[ sm.phase ]); debugm(" ");
  #endif
  }

StateXtionFnPtr_ _NULL_xtion(StateMachine &sm) { return NULL; }
const StateXtionFnPtr_ NOPREDS[] = { (StateXtionFnPtr_) NULL };
#define XTIONNAME(action) _##action##_xtion
#if DEBUG==1
  #define SIMPLESTATE(action, next_state) StateXtionFnPtr_ _##action##_xtion(StateMachine &sm) { \
      debug_time(); debug_phase(sm); debugm(F(#action)); debugm(F("\n")); \
      auto rez = one_step(sm, action_function_wrapper<action>, _##action##_xtion, NOPREDS, _##next_state##_xtion); \
      /* debugm(F("  -> ")); debugm(rez == _##action##_xtion ? #action : #next_state);debugm(F("\n")); */ \
      return rez; \
      }
#else
    #define SIMPLESTATE(action, next_state) StateXtionFnPtr_ _##action##_xtion(StateMachine &sm) { \
      return one_step(sm, action_function_wrapper<action>, _##action##_xtion, NOPREDS, _##next_state##_xtion); \
    }
#endif

#if DEBUG==1
  #define SIMPLESTATEAS(name, action, next_state) StateXtionFnPtr_ _##name##_xtion(StateMachine &sm) { \
      debug_time(); debug_phase(sm); debugm(F(#name":"#action)); debugm(F("\n")); \
      auto rez = one_step(sm, action_function_wrapper<action>, _##name##_xtion, NOPREDS, _##next_state##_xtion); \
      /* debugm(F("  -> ")); debugm(rez == _##name##_xtion ? F(#name":"#action) : F(#next_state)); debugm(F("\n")); */ \
      return rez; \
      }
#else
  #define SIMPLESTATEAS(name, action, next_state) StateXtionFnPtr_ _##name##_xtion(StateMachine &sm) { \
    return one_step(sm, action_function_wrapper<action>, _##name##_xtion, NOPREDS, _##next_state##_xtion); \
    }
#endif

#define STATEAS(name, action, next_state) StateXtionFnPtr_ _##name##_xtion(StateMachine &sm) { \
    static const ActionFnPtr _action = action_function_wrapper<action>; \
    static const StateXtionFnPtr next_state_xtion = _##next_state##_xtion; \
    static const StateXtionFnPtr self = _##name##_xtion; \
    static const StateXtionFnPtr_ preds[] = {
#define STATE(action, next_state) StateXtionFnPtr_ _##action##_xtion(StateMachine &sm) { \
    static const ActionFnPtr _action = action_function_wrapper<action>; \
    static const StateXtionFnPtr next_state_xtion = _##next_state##_xtion; \
    static const StateXtionFnPtr self = _##action##_xtion; \
    static const StateXtionFnPtr_ preds[] = {
#define GOTOWHEN(pred, action) gotowhen<pred, _##action##_xtion>,
#define END_STATE (StateXtionFnPtr_) NULL \
        }; \
    /* // template statxtinfntpr foraction(booleanfnpt action), and with statemachine &, and with phase */ \
    return one_step(sm, _action, self, preds, next_state_xtion); \
    }
#define RESTART(machine, action) machine.restart(_##action##_xtion)

StateXtionFnPtr_ one_step(StateMachine &sm, ActionFnPtr action, StateXtionFnPtr fromxtion, const StateXtionFnPtr_ preds[], StateXtionFnPtr nextxtion) {
    boolean again = (*action)(sm);
    // test preds at then end of every trial
    StateXtionFnPtr *pred = (StateXtionFnPtr*)preds; // head of list
    while(*pred) { // till NULL
        StateXtionFnPtr rez = (**pred)(sm); // result is either false or a fnptr
        if (rez) { 
          sm.phase = SM_Finish;
          (*action)(sm); // count on the wrappers to inhibit as necessary
          sm.phase = SM_Start;
          return rez; 
          };
        pred++;
        }
    if ( again ) {
        return fromxtion;
        }
    else {
        sm.phase = SM_Finish;
        (*action)(sm); // count on the wrappers to inhibit as necessary
        sm.phase = SM_Start;
        return nextxtion;
        }
    }

template<const int ms> boolean sm_delay(StateMachine &sm) {
    // we "stay" in this state till expired, so we can use the user_data
    if (sm.phase == SM_Start) {
        sm.user_long = millis() + ms;
        // I suppose we could have expired, e.g. 0ms
        }
        
    if (millis() >= sm.user_long) {
        sm.user_long = millis() + ms;
        return false;
        }
    return true; // again
    }

// have to declare for ourselves to make this work
// void digitalWrite(int, int);
// template<void (&fn)(int, int), int a, int b> boolean sm_as_action(StateMachine &sm) { fn(a,b); return false; }
// auto x = sm_as_action<digitalWrite, 13, HIGH>;
// StateXtionFnPtr y = x;

template<int pin, int v> void sm_digitalWrite() { digitalWrite(pin, v); }

template <int msg> void sm_msg() { Serial.println(msg); }
template <const char *&msg> void sm_msg() { Serial.println(*msg); }
template <const char msg[]> void sm_msg() { Serial.println(msg); }


#define STATEMACHINE(machinename, firstaction) StateMachine machinename(_##firstaction##_xtion);

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

// Some boolean combinators for predicates, since we can't do an expression
// Must enclose in () in the macros! ugly
// eg.. INTERRUPT_WHEN( (&SM_and<onhook, offhook>), play_message)
typedef boolean (&SimplePredicate)();
template <SimplePredicate a, SimplePredicate b> boolean SM_and() { return a() && b(); }
template <SimplePredicate a> boolean SM_not() { return !a(); }

boolean _FOREVER_xtion(StateMachine &sm) { return true; } // assume you'll use a GOTOWHEN

template<unsigned long at> boolean startup_delay() {
  // wait till clock is "at"
  return millis() >= at;
  }
