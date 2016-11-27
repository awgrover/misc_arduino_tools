#include "state_machine.h"

// preds. says squawk every 5secs, bawk every 9
/* should look about like:
    start-smstart
    <setup
    start
    start-smfinish
    3723 tic
    5052 squawk
    7396 tic
    9051 bawk
    9051 squawk
    10053 squawk
    11069 tic
*/

// the loop goes to A_top, A_start only once
SIMPLESTATE(A_start, A_top)
SIMPLESTATE(A_top, A_second)
STATE(A_second, A_third)
    GOTOWHEN(every5seconds, squawk)
    GOTOWHEN(everymillis<9000>, bawk)
    GOTOWHEN(nthTime<65535>, tic)
END_STATE
SIMPLESTATE(A_third, A_top)
SIMPLESTATE(squawk, A_top)
SIMPLESTATE(bawk, squawk)
SIMPLESTATE(tic, A_third)

STATEMACHINE(Machine_A,  A_start) 

//
//
//

void setup() {
    Serial.begin(9600);
    Serial.print("SM_Start, SM_Running, SM_Finish\n"); Serial.print(SM_Start);Serial.print(", "); Serial.print( SM_Running);Serial.print(", "); Serial.print( SM_Finish);Serial.print("\n");
    Serial.print("setup starting at ");Serial.print((long)A_top);Serial.print(" as ");Serial.println((long)XTIONNAME(A_top));
    Machine_A.run();
    Serial.println("<setup");
    }

void loop() {
     Machine_A.run();
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
boolean A_third(StateMachine& sm, StateMachinePhase phase) {
    if (phase == SM_Start) { return true;}
    else if (phase == SM_Running) { return false;}
    else if (phase == SM_Finish) { return false; }
    }
boolean squawk(StateMachine& sm, StateMachinePhase phase) {
    if (phase == SM_Start) { return true;}
    else if (phase == SM_Running) { Serial.print(millis());Serial.println(" squawk");  return false;}
    else if (phase == SM_Finish) { return false; }
    }
boolean bawk(StateMachine& sm, StateMachinePhase phase) {
    if (phase == SM_Start) { return true;}
    else if (phase == SM_Running) { Serial.print(millis());Serial.println(" bawk");  return false;}
    else if (phase == SM_Finish) { return false; }
    }
boolean tic(StateMachine& sm, StateMachinePhase phase) {
    if (phase == SM_Running) { Serial.print(millis());Serial.println(" tic");  return false;}
    return true;
    }
boolean every5seconds() {
    static unsigned long every = millis() + 5000;
    // Serial.println(every-millis());
    if (millis() > every) {
        every = millis() + 5000;
        return true;
        }
    return false;
    }
boolean every9seconds() {
    static unsigned long every = millis() + 9000;
    // Serial.println(every-millis());
    if (millis() > every) {
        every = millis() + 9000;
        return true;
        }
    return false;
    }
