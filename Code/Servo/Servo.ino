/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Sweep
*/ 

#include <ESP32_Servo.h>
Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 
int pos = 0;    // variable to store the servo position 
String incomingStr = "";
byte flag = 0;
void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo.write(0);
  Serial.begin(9600);
} 
 
void loop() 
{ 
  if (Serial.available() != 0) {

  incomingStr = Serial.readString(); // read the incoming byte:

  Serial.print(" I received:");
  Serial.println(incomingStr);
  if (incomingStr!="" && flag == 0){
    flag = 1;
  }
  else if (incomingStr!="" && flag == 1){
    flag = 0;
  }
  if (flag == 1){
    myservo.write(120);
  }
  else if (flag == 0){
    myservo.write(0);
  }
  
  
  }
}
