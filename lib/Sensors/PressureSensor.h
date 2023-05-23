#pragma once

#include <Adafruit_DPS310.h>

class PressureSensor {
public:
    PressureSensor();

    bool healthy() const;

    void read();

    float get_pressure() const;
    
    float get_temperature() const;
    
private:
    sensors_event_t _pressure_event;
    sensors_event_t _temperature_event;

    Adafruit_DPS310 _dps;
    
    const int max_init_retries = 100;
    bool _healthy;
};