#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// This struct contains anything that 
class Device {
public:
    Device() : pixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800),
               serial(Serial) {
        serial.begin(9600);
        pixel.begin();
    }

    void showColour(uint32_t colour) {
        pixel.setPixelColor(0, colour);
        pixel.show();
    }

    USBCDC serial;
    
private:
    Adafruit_NeoPixel pixel;
};