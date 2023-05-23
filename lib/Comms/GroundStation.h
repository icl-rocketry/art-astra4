#pragma once

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>


class GroundStation {
public:
    GroundStation() : server_(80), done(false) {}

    bool connect_to_wifi();
    void run();

private:
    bool done;

    void kill();
    WebServer server_;
};