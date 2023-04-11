#include "StateMachine.h"
#include <ApogeeDetector.h>

void StateMachine::run() {
    while (true) {
        State* new_state = state->run();
        if (new_state == nullptr) return;
        delete state; // Manual memory management (my favourite)
        state = new_state;
    }
}

State* Diagnostic::run_() {
    if (!device.healthy()) {
        return new Error("DPS310 initialisation failed", device);
    }
    return new Preflight(device);
}

State* Preflight::run_() {
    auto& ground_station = device.get_ground_station();
    
    if (!ground_station.connect_to_wifi()) {
        return new Error("Couldn't connect to wifi", device);
    }
    
    ground_station.run();
    
    return new FlightPreApogee(device);
}

State* FlightPreApogee::run_() {
    ApogeeDetector detector(millis(), 20);
    PressureSensor& sensor = device.get_pressure_sensor();

    bool done = false;

    while (!done) {
        sensor.read();
        done = detector.detect(millis(), sensor.get_pressure());
    }

    return new Separation(device);
}

State* Separation::run_() {
    device.start_servo_forward();
    delay(5000);
    device.start_servo_backward();
    delay(5000);
    device.stop_servo();
    return new PostFlight(device);
}

// Don't do anything
State* PostFlight::run_() {
    while (true) {
        delay(10000);
    }
    return nullptr;
}

State* Error::run_() {
    //TODO: add logger to log the error
    while (true) {
        device.serial.println("TODO");
        delay(1000);
    }
    return nullptr;
}
