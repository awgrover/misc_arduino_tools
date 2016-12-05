/* 
    A "no setup" example. We read the analog pin which is floating. Might be interesting. 
    Watch the graph, then move your and near the arduino. Etc.

    Wiring: none! 
*/

#include "auto_bilevel.h"

#define PIN A0

void setup() {
    Serial.begin(115200);
    pinMode(PIN, INPUT); // definitely no pullup, we want it to float
    for (int i=0; i<100; i++) { analogRead(PIN); } // stabilize
    }

void loop() {
    int raw = analogRead(PIN);
    // need to smooth twice because the floating pin is a mess
    long smooth1 = exponential_smooth(raw, 7);

    // Adjust these numbers after looking at the graph.
    boolean level2 = auto_bilevel_graph(smooth1, 23, 81, 2);

    delay(20); // breaks the plotter if we are too fast!
    }
