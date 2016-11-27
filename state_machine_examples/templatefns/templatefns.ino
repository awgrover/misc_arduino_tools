#include "state_machine.h"

// use template functions
/* should blink the LED and look like:
    start-smstart
    start
    start-smfinish
    2052 tic
    4052 tic
    6052 tic
    8052 tic
    8052 tic2
    10052 tic
    12052 tic
    12052 tic2
*/

#define LED 13

// the loop goes to A_top, A_start only once

SIMPLESTATE(A_start, A_top)
SIMPLESTATE(A_top, delay1)
SIMPLESTATEAS(delay1, sm_delay<2000>, tic)
STATE(A_second, A_top) 
    GOTOWHEN((SM_and< nthTime<4>, everymillis<1000> >), tic2) // funny behavior: 16k, then every 8k
END_STATE
SIMPLESTATE(tic, A_second)
SIMPLESTATE(tic2, A_top)

// another machine
SIMPLESTATEAS(digitalBhigh, (sm_digitalWrite<LED, HIGH>), delay500)
SIMPLESTATEAS(delay500, sm_delay<500>, digitalBlow)
SIMPLESTATEAS(digitalBlow, (sm_digitalWrite<LED, LOW>), delay501)
SIMPLESTATEAS(delay501, sm_delay<500>, digitalBhigh)



STATEMACHINE(Machine_A, A_start);
STATEMACHINE(Machine_Blink, digitalBlow);

//
//
//

void setup() {
    pinMode(LED,OUTPUT);
    Serial.begin(9600);
    Serial.print("SM_Start, SM_Running, SM_Finish\n"); Serial.print(SM_Start);Serial.print(", "); Serial.print( SM_Running);Serial.print(", "); Serial.print( SM_Finish);Serial.print("\n");
    Serial.print("setup starting at ");Serial.print((long)A_top);Serial.print(" as ");Serial.println((long)XTIONNAME(A_top));
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
