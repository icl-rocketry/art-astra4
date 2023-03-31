#include "GroundStation.h"
#include <Arduino.h>

#define SSID "ASTRA4"
#define PASSWORD "password" // We love security
#define MAX_CONNECTION_RETRIES 100

bool GroundStation::connect_to_wifi() {
    Serial.print("Connecting to ");
    Serial.println(SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    
    int i = 0;
    while (WiFi.status() != WL_CONNECTED && i < MAX_CONNECTION_RETRIES) {
        delay(500);
        Serial.print(".");
        i++;
    }
    
    // Can access this board @ "http://separation.local/"
    if (MDNS.begin("separation")) {
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