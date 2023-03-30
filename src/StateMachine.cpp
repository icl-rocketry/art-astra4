#include <Adafruit_NeoPixel.h>

int DATA_RATE = 10; // 10 Hz
float TIME_INTERVAL_DATA_TRANSFER = 1 / DATA_RATE;
float RECORDING_TIME = 15 * 60;
float N_RECORDINGS = RECORDING_TIME * DATA_RATE;
float PREFLIGHT_WAIT_TIME = 30 * 60;

#define NUMPIXELS 1
#define PIN_NEOPIXEL 2

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

enum State {
  PREFLIGHT,
  DIAGNOSTIC,
  FLIGHT_PRE_APOGEE,
  SEPARATION,
  POST_FLIGHT
};

class StateMachine {
  private:
    State currentState = PREFLIGHT;
    unsigned long stateStartTime = 0;

    diagnostic diagnosticState;
    preflight preflightState;
    flight_pre_apogee flightPreApogeeState;
    separation separationState;
    post_flight postFlightState;

  public:
    void run() {
      switch(currentState) {
        case PREFLIGHT:
          preflightState.preflighting();
          if (millis() - stateStartTime > PREFLIGHT_WAIT_TIME * 1000) {
            currentState = DIAGNOSTIC;
            stateStartTime = millis();
          }
          break;

        case DIAGNOSTIC:
          diagnosticState.diagnosing();
          if (millis() - stateStartTime > N_RECORDINGS * TIME_INTERVAL_DATA_TRANSFER * 1000) {
            currentState = FLIGHT_PRE_APOGEE;
            stateStartTime = millis();
          }
          break;

        case FLIGHT_PRE_APOGEE:
          flightPreApogeeState.flightPreApogeeing();
          // we need to add code to transition to separation state
          break;

        case SEPARATION:
          separationState.separating();
          // we need to add code to transition to post flight state
          break;

        case POST_FLIGHT:
          postFlightState.post_flighting();
          break;
      }
    }
};

StateMachine stateMachine;

void setup() {
  Serial.begin(115200);

#if defined(NEOPIXEL_POWER)
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
#endif

  pixels.begin();
  pixels.setBrightness(20);

  stateMachine.run();
}

void loop() {
  stateMachine.run();
}

class diagnostic {
  public:
    void diagnosing() {
      pixels.fill(255, 255, 0); // Yellow
      pixels.show();
    }
};

class preflight {
  public:
    void preflighting() {
      pixels.fill(255, 0, 255); // Magenta
      pixels.show();
    }
};

class flight_pre_apogee {
  public:
    void flightPreApogeeing() {
      pixels.fill(0, 255, 0); // Green
      pixels.show();
    }
};

class separation {
  public:
    void separating() {
      pixels.fill(0, 0, 255); // Blue
      pixels.show();
    }
};

class post_flight {
  public:
    void post_flighting() {
      pixels.fill(255, 255, 255); // White
      pixels.show();
    }

};
