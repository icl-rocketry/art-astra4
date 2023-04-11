#include "StateMachine.h"
#include <ApogeeDetector.h>

#define DATA_LOG_FILE "/data.bin"

void StateMachine::run() {
    while (true) {
        State* new_state = state->run();
        if (new_state == nullptr) break;
        delete state; // Manual memory management (my favourite)
        state = new_state;
    }
    while (true) {
        delay(10000);
    }
}

State* Diagnostic::run_() {
    auto& dps = device.get_pressure_sensor();
    if (!dps.healthy()) {
        return new Error("DPS310 initialisation failed", device);
    }

    if (filesys::exists(DATA_LOG_FILE)) {
        return new Error("Data log file exists", device);
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

    File32 data_file = filesys::open(DATA_LOG_FILE, filesys::WRITEONLY);
    Logger data_logger(data_file);

    bool done = false;

    uint32_t time;
    float pressure;

    while (!done) {
        sensor.read();
        time = millis();
        pressure = sensor.get_pressure();
        done = detector.detect(time, pressure);
        data_logger.log(time, pressure);
    }

    data_file.close();

    device.sys_log("Apogee detected @ ", std::to_string(pressure), "Pa");
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
    return nullptr;
}

State* Error::run_() {
    device.sys_log(msg);
    return nullptr;
}
