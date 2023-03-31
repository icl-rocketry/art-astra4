#include "StateMachine.h"

void StateMachine::run() {
    while (true) {
        State* new_state = state->run();
        if (new_state == nullptr) return;
        delete state; // Manual memory management (my favourite)
        state = new_state;
    }
}

State* Diagnostic::run_() {
    // TODO: Fill this out
    return new Preflight(device);
}

State* Preflight::run_() {
    if (!device.ground_station.connect_to_wifi()) {
        return new Error("Couldn't connect to wifi", device);
    }
    device.ground_station.run();
    device.ground_station.kill();
    // TODO: set the flight start time to now
    return new FlightPreApogee(device);
}

State* FlightPreApogee::run_() {
    // TODO: Fill this out
    return new Separation(device);
}

State* Separation::run_() {
    // TODO: Fill this out
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
