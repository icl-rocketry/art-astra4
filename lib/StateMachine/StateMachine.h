#pragma once

#include "Device.h"

// I'm not an expert in colours so these *might* be wrong
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define YELLOW 0xFFFF00
#define CYAN 0x00FFFF
#define MAGENTA 0xFF00FF
#define WHITE 0xFFFFFF

class State {
public:
    State(Device& device, uint32_t colour) : device(device), 
                                             colour(colour) {}

    State* run() {
        device.showColour(colour);
        return run_();
    }

protected:
    virtual State* run_();

    Device& device;
    uint32_t colour;
};

class Diagnostic : public State {
public:
    Diagnostic(Device& device) : State(device, BLUE) {}

protected:
    State* run_();
};

class Preflight : public State {
public:
    Preflight(Device& device) : State(device, GREEN) {}

protected:
    State* run_();
};

class FlightPreApogee : public State {
public:
    FlightPreApogee(Device& device) : State(device, WHITE) {}

protected:
    State* run_();
};

class Separation : public State {
public:
    Separation(Device& device) : State(device, MAGENTA) {}

protected:
    State* run_();
};

class PostFlight : public State {
public:
    PostFlight(Device& device) : State(device, CYAN) {}

protected:
    State* run_();
};


class Error : public State {
public:
    Error(const std::string msg, Device& device) : State(device, RED), msg(msg) {}

protected:
    State* run_();

private:
    std::string msg;
};



class StateMachine {
public:
    StateMachine(State* initial) : state(initial) {}

    void run();

private:
    Device device;
    State* state;
};