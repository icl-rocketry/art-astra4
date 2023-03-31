#include "GroundStation.h"
#include <Arduino.h>

#define SSID "ASTRA4"
#define PASSWORD "password" // We love security

void GroundStation::connect_to_wifi() {
    Serial.print("Connecting to ");
    Serial.println(SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    // This was in the tutorial - no clue what it does (https://archive.ph/OQl4H)
    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }


    // Request handling code
    server_.on("/go", [this]() {
        server_.send(200, "text/plain", "yes");
        done = true;
    });

    server_.on("/health", [this]() {
        server_.send(200, "text/plain", "healthy");
    });

    server_.begin();
    Serial.println("Server started");
}

void GroundStation::run() {
    while (!done) {
        server_.handleClient();
        delay(2);
    }
}

void GroundStation::kill() {
    server_.stop();
    MDNS.end();
    WiFi.disconnect(true);
}