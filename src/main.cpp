#include <Arduino.h>
#include <Device.h>
#include <StateMachine.h>

Device device;
StateMachine sm(device, new Diagnostic(device));

void setup() {}

void loop() {
  sm.run();
}