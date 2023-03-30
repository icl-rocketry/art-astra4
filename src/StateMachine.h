#pragma once

#include "Device.h"

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

class RedState : public State {
public:
    RedState(Device& device) : State(device, 0xff0000) {}

protected:
    State* run_() override;
};

class GreenState : public State {
public:
    GreenState(Device& device) : State(device, 0x00ff00) {}

protected:
    State* run_() override;

};
class BlueState : public State {
public:
    BlueState(Device& device) : State(device, 0x0000ff) {}

protected:
    State* run_() override;
};

class StateMachine {
public:
    StateMachine(Device& device, State* initial) : device(device), state(initial) {}

    void run() {
        while (true) {
            state = state->run();
        }
    }

private:
    Device& device;
    State* state;
};