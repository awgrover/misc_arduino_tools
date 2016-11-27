// #include "state_machine.h"

// v2 template based
/*
    template wrapper for things like digitalWrite, and STATEAS
    STATEAS
    test phases!
    test end on null
*/

#define DEBUG 0
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

    boolean run() {
        debugm("Step action ");debugm((long)current);debugm(" @");debugm(phase);debugm("...");

        // someone might try to run us after we signaled "all done"
        if (current == NULL) {
            return NULL;
            }

        // The StateXtionFn (_realstatename_xtion) returns "what to do next", which may be stay
        StateXtionFnPtr next_state = (*current)(*this);
      if (next_state != current) { 
        debugm(" !->");debugm((long)next_state);debugm("\n"); 
        }
      else { 
        phase = SM_Running;
        debugm(" again\n"); 
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
template<boolean fn(StateMachine &sm)> inline boolean action_function_wrapper(StateMachine &sm) { return fn(sm); }
template<boolean fn(StateMachinePhase phase)> inline boolean action_function_wrapper(StateMachine &sm) { return fn(sm.phase); }
template<boolean fn()> inline boolean action_function_wrapper(StateMachine &sm) { return fn(); }
template<void fn(StateMachine &sm, StateMachinePhase phase)> inline boolean action_function_wrapper(StateMachine &sm) { fn(sm, sm.phase); return false; }
template<void fn(StateMachine &sm)> inline boolean action_function_wrapper(StateMachine &sm) { fn(sm); return false;}
template<void fn()> inline boolean action_function_wrapper(StateMachine &sm) { fn(); return false; }

const StateXtionFnPtr_ NOPREDS[] = { (StateXtionFnPtr_) NULL };
#define SIMPLESTATE(action, next_state) StateXtionFnPtr_ _##action##_xtion(StateMachine &sm) { \
    return one_step(sm, action_function_wrapper<action>, _##action##_xtion, NOPREDS, _##next_state##_xtion); \
    }
#define SIMPLESTATEAS(name, action, next_state) StateXtionFnPtr_ _##name##_xtion(StateMachine &sm) { \
    return one_step(sm, action_function_wrapper<action>, _##name##_xtion, NOPREDS, _##next_state##_xtion); \
    }
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

StateXtionFnPtr_ one_step(StateMachine &sm, ActionFnPtr action, StateXtionFnPtr fromxtion, const StateXtionFnPtr_ preds[], StateXtionFnPtr nextxtion) {
    boolean again = (*action)(sm);
    // test preds at then end of every trial
    StateXtionFnPtr *pred = (StateXtionFnPtr*)preds; // head of list
    while(*pred) { // till NULL
        StateXtionFnPtr rez = (**pred)(sm); // result is either false or a fnptr
        if (rez) { return rez; };
        pred++;
        }
    if ( again ) {
        return fromxtion;
        }
    else {
        sm.phase = SM_Finish;
        // call w/finish
        sm.phase = SM_Start;
        return nextxtion;
        }
    }

template<const int ms> boolean delaying() {
    static unsigned long timer = millis() + ms;
    if (millis() >= timer) {
        timer = millis() + ms;
        return false;
        }
    return true; // again
    }

#define STATEMACHINE(machinename, firstaction) StateMachine machinename(_##firstaction##_xtion);

STATE(start, count) END_STATE
STATE(count, hello) END_STATE
STATE(hello, bawk)
    GOTOWHEN(every1000, squawk)
END_STATE
STATE(squawk, bawk) END_STATE
STATE(bawk, stay4) END_STATE
SIMPLESTATE(stay4, renamed1)
SIMPLESTATEAS(renamed1, tic501, step1)
STATE(step1, count) END_STATE

STATEMACHINE (m_a,hello);

SIMPLESTATE(machine2start, m2top)
SIMPLESTATE(m2top, waitfor401)
SIMPLESTATEAS(waitfor401, delaying<401>,m2bottom) // just names the templated
SIMPLESTATE(m2bottom, m2top)

STATEMACHINE(m_b, machine2start)

//
//
//

#define ONBOARDLED 13


void setup() {
    Serial.begin(9600);
    Serial.println("squawk every 100000, bawk every 500ms, stay4 by 3 every 500, tic501 every 501, m2bottom every 401");
    }

void loop() {
    m_a.run();
    m_b.run();
    }

unsigned long cycle_counter=0;

boolean start(StateMachine &sm) {
    Serial.println("start");
    return false;
    }

boolean count(StateMachine &sm) {
    cycle_counter ++;
    return false;
    }
boolean hello(StateMachine &sm) {
    // Serial.println("Hello");
    return false;
    }
boolean step1(StateMachine &sm) {
    // Serial.println("step1");
    return false;
    }

void On13() { digitalWrite(ONBOARDLED, HIGH); }
void Off13() { digitalWrite(ONBOARDLED, HIGH); }
boolean every1000() { 
    static unsigned long ct=10000ul; // on my UNO that's about every 400ms
    ct=ct-1;
    if (ct>0) { return false; } 
    ct=10000ul; 
    return true; 
    }

void signal(const char* msg) {
    Serial.print(msg); Serial.print(" "); Serial.print(cycle_counter); Serial.print(" "); Serial.println(millis());
    }

boolean squawk(StateMachine &sm) { signal("squawk "); return false;}

void bawk() {
    static unsigned long timer = millis() + 500;
    if (millis() >= timer) { 
        timer = millis() + 500; 
        signal("bawk"); 
        }
    }

boolean stay4() {
    static unsigned long timer = millis() + 500;
    static int ct = 4;
    static int countto4 = 0;
    if (millis() >= timer) { // at 500, stay for 4 times
        ct--;
        if (ct>0) { 
            countto4++;
            return true;
            } // stay
        ct=4; timer = millis() + 500;
        Serial.print("stay4 "); Serial.print(countto4); Serial.print(" ");Serial.print(cycle_counter); Serial.print(" "); Serial.println(millis());
        return false;
        }
    }

void tic501() {
    static unsigned long timer = millis() + 500;
    if (millis() >= timer) {
        timer = millis() + 501;
        signal("tic501");
        }
    }

void machine2start() { signal("machine 2 start"); }
void m2top() { }
void m2bottom() { signal("m2bottom"); }
