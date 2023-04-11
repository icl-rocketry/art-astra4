#include <Arduino.h>
#include <StateMachine.h>

void setup() {}

void loop() {
    Device device; // This is here because the constructor actually runs code
    StateMachine sm(device, new Diagnostic(device));

    sm.run();
}