#include "state_machine.h"

#define ONBOARDLED 13

// basic states
STATE(start, count) END_STATE
STATE(count, hello) END_STATE
STATE(hello, bawk)
    GOTOWHEN(every1000, squawk)
END_STATE
STATE(squawk, bawk) END_STATE
STATE(bawk, stay4) END_STATE
SIMPLESTATE(stay4, renamed1)
SIMPLESTATEAS(renamed1, tic501, laststep)
STATE(laststep, count) END_STATE

STATEMACHINE (m_a,hello);

// test delaying (and AS for a templat function)
SIMPLESTATE(machine2start, m2top)
SIMPLESTATE(m2top, waitfor401)
SIMPLESTATEAS(waitfor401, sm_delay<401>,m2bottom) // just names the templated
SIMPLESTATE(m2bottom, m2top)

STATEMACHINE(m_b, machine2start)

// Blink fast
STATEAS(ledon, (sm_digitalWrite<ONBOARDLED, HIGH>), ledblinkwait) END_STATE
STATEAS(ledblinkwait, sm_delay<50>, ledoff) END_STATE
SIMPLESTATEAS(ledoff, (sm_digitalWrite<ONBOARDLED, LOW>), ledblinkwait2)
SIMPLESTATEAS(ledblinkwait2, sm_delay<50>, ledon)

STATEMACHINE(m_led, ledon)

// Blink slow
SIMPLESTATE(slowison, slow_ledon)
STATEAS(slow_ledon, (sm_digitalWrite<ONBOARDLED, HIGH>), slow_ledblinkwait) END_STATE
STATEAS(slow_ledblinkwait, sm_delay<700>, slowisoff) END_STATE
SIMPLESTATE(slowisoff, slow_ledoff)
SIMPLESTATEAS(slow_ledoff, (sm_digitalWrite<ONBOARDLED, LOW>), slow_ledblinkwait2)
SIMPLESTATEAS(slow_ledblinkwait2, sm_delay<700>, slowison)

STATEMACHINE(m_slowled, slowison)

// This machine doesn't loop: "once through"


SIMPLESTATE(phases, check_phases)
SIMPLESTATE(check_phases, NULL)

STATEMACHINE(m_once, phases)
//
//
//


void setup() {
    pinMode(ONBOARDLED, OUTPUT);
    Serial.begin(9600);
    Serial.println("squawk every 100000, bawk every 500ms, stay4 by 3 every 500, tic501 every 501, m2bottom every 401, led blinks: rapid, pause, rapid");
    }

boolean allow_fast = false;

void loop() {
    m_a.run();
    m_b.run();
    if (allow_fast) m_led.run(); // only run while slow is on
    m_slowled.run();
    m_once.run();
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
boolean laststep(StateMachine &sm) {
    // Serial.println("laststep");
    return false;
    }

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

void slowisoff() { allow_fast = false; }
void slowison() { allow_fast = true; }

int phases_callnum = 0;
boolean phases(StateMachinePhase phase) {
    if (phase == SM_Start && phases_callnum == 0) {
        phases_callnum = 1;
        return true;
        }
    else if (phase == SM_Running && phases_callnum == 1) {
        phases_callnum = 2;
        return false; // done
        }
    else if (phase == SM_Finish && phases_callnum == 2) {
        phases_callnum = 3;
        return false; // na
        }
    Serial.print("Wrong sequence, was at call #");Serial.print(phases_callnum);Serial.print(" saw phase ");Serial.println(phase);
    return false;
    }

void check_phases() {
    if (phases_callnum != 3) {
        Serial.print("Finished with wrong call #");Serial.println(phases_callnum);
        }
    else { signal("Phases worked"); }
    }
