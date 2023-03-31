#pragma once

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>


class GroundStation {
public:
    GroundStation() : server_(80), done(false) {}
    ~GroundStation() {
        kill();
    }


    void connect_to_wifi();
    void run();
    void kill();

private:
    bool done;

    WebServer server_;
};