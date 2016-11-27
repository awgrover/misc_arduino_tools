#include "state_machine.h"

// A. sequence
/* should look like:
    start-smstart
    <setup
    start
    start-smfinish
    top-smstart
    top
    top-smfinish
    second-smstart
    second
    second
    second
    second-smfinish
    third-smstart
    third
    third-smfinish
*/

// the loop goes to A_top, A_start only once
SIMPLESTATE(A_start, A_top)
SIMPLESTATE(A_top, A_second)
SIMPLESTATE(A_second, A_third)
SIMPLESTATE(A_third, A_top)

STATEMACHINE(Machine_A, A_start);

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
    if (phase == SM_Start) { Serial.println("top-smstart"); return true;}
    else if (phase == SM_Running) { Serial.println("top");  return false;}
    else if (phase == SM_Finish) { Serial.println("top-smfinish"); return false; }
    }
boolean A_second(StateMachine& sm, StateMachinePhase phase) {
    static int ct;
    if (phase == SM_Start) { Serial.println("second-smstart"); ct=2; return true;}
    else if (phase == SM_Running) { Serial.println("second");  return ct--;} // count down
    else if (phase == SM_Finish) { Serial.println("second-smfinish"); return false; }
    }
boolean A_third(StateMachine& sm, StateMachinePhase phase) {
    if (phase == SM_Start) { Serial.println("third-smstart"); return true;}
    else if (phase == SM_Running) { Serial.println("third");  return false;}
    else if (phase == SM_Finish) { Serial.println("third-smfinish"); return false; }
    }

