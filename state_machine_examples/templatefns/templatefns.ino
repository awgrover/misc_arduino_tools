#include "state_machine.h"

// use template functions
// 

#define LED 13

// the loop goes to A_top, A_start only once

STATE(A_start)
    WHEN_DONE(A_top)
END_STATE
STATE(A_top)
    WHEN_DONE(delay1)
END_STATE

auto delay1 = sm_delay<2000>;
STATE(delay1) WHEN_DONE(tic) END_STATE

STATE(A_second) 
    INTERRUPT_WHEN((SM_and< nthTime<4>, everymillis<1000> >), tic2) // funny behavior: 16k, then every 8k
    WHEN_DONE(A_top) 
END_STATE

STATE(tic)
    WHEN_DONE(A_second)
END_STATE

STATE(tic2) WHEN_DONE(A_top) END_STATE

// another machine
auto digitalBhigh = sm_digitalWrite<LED, HIGH>;
STATE(digitalBhigh) WHEN_DONE(delay500) END_STATE

auto delay500 = sm_delay<500>;
STATE(delay500) WHEN_DONE(digitalBlow) END_STATE

auto digitalBlow = sm_digitalWrite<LED, LOW>;
STATE(digitalBlow) WHEN_DONE(delay501) END_STATE

auto delay501 = sm_delay<500>;
STATE(delay501) WHEN_DONE(digitalBhigh) END_STATE



StateMachine Machine_A( STATE_NAME(A_start) );
StateMachine Machine_Blink( STATE_NAME(digitalBlow) );

//
//
//

void setup() {
    pinMode(LED,OUTPUT);
    Serial.begin(9600);
    Serial.print("SM_Start, SM_Running, SM_Finish\n"); Serial.print(SM_Start);Serial.print(", "); Serial.print( SM_Running);Serial.print(", "); Serial.print( SM_Finish);Serial.print("\n");
    Serial.print("setup starting at ");Serial.print((long)A_top);Serial.print(" as ");Serial.println((long)STATE_NAME(A_top));
    // Machine_A.run();
    Serial.println("<setup");
    }

void loop() {
     Machine_A.run();
     Machine_Blink.run();
     }

boolean A_start(StateMachine& sm, StateMachinePhase phase) { 
    if (phase == SM_Start) { Serial.println("start-smstart"); return true;}
    else if (phase == SM_Running) { Serial.println("start");  return false;}
    else if (phase == SM_Finish) { Serial.println("start-smfinish"); return false; }
    }
boolean A_top(StateMachine& sm, StateMachinePhase phase) {
    if (phase == SM_Start) { return true;}
    else if (phase == SM_Running) { return false;}
    else if (phase == SM_Finish) { return false; }
    }
boolean A_second(StateMachine& sm, StateMachinePhase phase) {
    if (phase == SM_Start) { return true;}
    else if (phase == SM_Running) { return false;}
    else if (phase == SM_Finish) { return false; }
    }
boolean tic(StateMachine& sm, StateMachinePhase phase) {
    if (phase == SM_Running) { Serial.print(millis());Serial.println(" tic");  return false;}
    return true;
    }
boolean tic2(StateMachine& sm, StateMachinePhase phase) {
    if (phase == SM_Running) { Serial.print(millis());Serial.println(" tic2");  return false;}
    return true;
    }
