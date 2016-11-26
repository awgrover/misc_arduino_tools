#include "state_machine.h"

// preds. says squawk every 5secs, bawk every 9

// the loop goes to A_top, A_start only once
STATE(A_start)
    WHEN_DONE(A_top)
END_STATE
STATE(A_top)
    WHEN_DONE(A_second)
END_STATE
STATE(A_second)
    INTERRUPT_WHEN(every5seconds, squawk)
    INTERRUPT_WHEN(everymillis<9000>, bawk)
    INTERRUPT_WHEN(nthTime<65535>, tic)
    WHEN_DONE(A_third)
END_STATE
STATE(A_third)
    WHEN_DONE(A_top)
END_STATE
STATE(squawk)
    WHEN_DONE(A_top)
END_STATE
STATE(bawk)
    WHEN_DONE(squawk)
END_STATE
STATE(tic)
    WHEN_DONE(A_third)
END_STATE

StateMachine Machine_A( STATE_NAME(A_start) );

//
//
//

void setup() {
    Serial.begin(9600);
    Serial.print("SM_Start, SM_Running, SM_Finish\n"); Serial.print(SM_Start);Serial.print(", "); Serial.print( SM_Running);Serial.print(", "); Serial.print( SM_Finish);Serial.print("\n");
    Serial.print("setup starting at ");Serial.print((long)A_top);Serial.print(" as ");Serial.println((long)STATE_NAME(A_top));
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
