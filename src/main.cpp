#include <Arduino.h>
#include <Device.h>
#include <StateMachine.h>

Device device;
StateMachine sm(device, new RedState(device));

void setup() {}

void loop() {
  sm.run();
}