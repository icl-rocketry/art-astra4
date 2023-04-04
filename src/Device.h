#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include "GroundStation.h"

#define SERVO_PIN A3
#define SERVO_ENABLE A2

class Device {
public:
    Device() : pixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800),
               serial(Serial) {

        // No idea why I'm doing this but that's what the examples for the ESP32 servo library want
        ESP32PWM::allocateTimer(0);
        ESP32PWM::allocateTimer(1);
        ESP32PWM::allocateTimer(2);
        ESP32PWM::allocateTimer(3);


        serial.begin(9600);
        pixel.begin();
        
        //544 and 2400 are the default pulse ranges from the normal arduino lib
        myservo.attach(A3, 544, 2400);
        
        digitalWrite(SERVO_ENABLE, HIGH);
        myservo.write(90);
    }

    void showColour(uint32_t colour) {
        pixel.setPixelColor(0, colour);
        pixel.show();
    }

    void start_servo_forward() {
        digitalWrite(SERVO_ENABLE, HIGH);
        myservo.write(45);
    }
    
    void stop_servo() {
        digitalWrite(SERVO_ENABLE, HIGH);
        myservo.write(90);
        digitalWrite(SERVO_ENABLE, LOW);
    }

    void start_servo_backward() {
        digitalWrite(SERVO_ENABLE, HIGH);
        myservo.write(135);
    }

    uint32_t start_time;
    USBCDC serial;
    GroundStation ground_station;
    
private:
    Adafruit_NeoPixel pixel;
    Servo myservo;
};