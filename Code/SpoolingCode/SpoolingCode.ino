// Code for the Spooling Software 

// Calculating when to trigger the Seperation Software. 

// Wait 5s after separation to trigger the Spooling part.
#include <Arduino.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
} 

void loop() {
  if (Serial.available()) {
    int pos = Serial.parseInt();
    myservo.write(pos);
    delay(100);
  }
}
