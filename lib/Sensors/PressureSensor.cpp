#include "PressureSensor.h"

PressureSensor::PressureSensor() {
    _healthy = true;
    
    Wire1.setPins(SDA1, SCL1);

    int retries = 0;
    while (retries < max_init_retries && !_dps.begin_I2C(0x77, &Wire1)) {
        delay(10);
        retries++;
    }
    if (retries == max_init_retries) {
        _healthy = false;
    }

    // Record pressure and temperature
    _dps.setMode(DPS310_CONT_PRESTEMP);

    // This should give us 8 samples per second
    _dps.configurePressure(DPS310_128HZ, DPS310_16SAMPLES);
    _dps.configureTemperature(DPS310_128HZ, DPS310_16SAMPLES);
}

bool PressureSensor::healthy() const {
    return _healthy;
}

void PressureSensor::read() {
    while (!_dps.temperatureAvailable() || !_dps.pressureAvailable()) {
        delay(5);
    }

    _dps.getEvents(&_temperature_event, &_pressure_event);
}

float PressureSensor::get_pressure() const {
    return _pressure_event.pressure;
}

float PressureSensor::get_temperature() const {
    return _temperature_event.temperature;
}