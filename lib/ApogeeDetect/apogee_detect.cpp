

#include "apogee_detect.h"

#include <Arduino.h>
#include <ArduinoEigenDense.h>

#include <iostream>
#include <sstream>
#include <vector>
// #include "millis.h"

ApogeeDetect::ApogeeDetect(uint16_t sampleTime) : _sampleTime(sampleTime),
                                                  mlock(true),  // intialise the mach lockout stuff
                                                  _apogeeinfo({false, 0, 0}) {
}

// Function to update flight data values and return data for apogee prediction
const ApogeeInfo &ApogeeDetect::checkApogee(float altitude, float velocity, uint32_t time) {
    if (!initialEntryTime) {
        initialEntryTime = time;  // recording first time this method is called to scale the system better
    }

    // If this is called too fast, don't do anything
    if (millis() - prevCheckApogeeTime <= _sampleTime) {
        return _apogeeinfo;
    }

    uint32_t timeSinceEntry = time - initialEntryTime;

    uint32_t prevTime = time_array.pop_push_back(timeSinceEntry);
    float prevAltitude = altitude_array.pop_push_back(altitude);

    // Mach lock check: (Don't do anything while going too fast)
    if (abs(velocity) >= mlock_speed) {
        if (!mlock) {
            // _logcontroller.log("Mach Lock Triggered!");
        }
        mlock = true;
        prevMachLockTime = millis();

        // log time
    } else if ((millis() - prevMachLockTime) > mlock_time) {  // if more than a second has passed and vel is less than mlock_speed
        mlock = false;
        // _logcontroller.log("Mach unlocked");
        // log the time
    }

    // Apogee detection:
    if (!(_apogeeinfo.reached)) {
        // coeffs = poly2fit(time_array, altitude_array); // POLYFIT -> x(t), time in ms

        // Fit a quadratic to the data
        quadraticFit((float)prevTime / 1000.0, (float)timeSinceEntry / 1000.0, prevAltitude, altitude);

        _apogeeinfo.time = ((-coeffs(1) / (2 * coeffs(2))) * 1000) + initialEntryTime;  // maximum from polyinomial using derivative
        // _logcontroller.log(std::to_string(_apogeeinfo.time));
        // _logcontroller.log("coeffs: " + std::to_string(coeffs(0)) + " " + std::to_string(coeffs(1)) + " " + std::to_string(coeffs(2)));

        if ((millis() >= _apogeeinfo.time) && (coeffs(2) < 0) && (millis() > 0) && (altitude > alt_min) && !mlock) {
            _apogeeinfo.altitude = coeffs(0) - (std::pow(coeffs(1), 2) / (4 * coeffs(2)));
            // _logcontroller.log("predicted apogee" + std::to_string(_apogeeinfo.time));
            // coeffs(2) * std::pow(_apogeeinfo.time,2) + (coeffs(1) * _apogeeinfo.time) + coeffs(0); // evalute 2nd order polynomial
            // _logcontroller.log(std::to_string(altitude - _apogeeinfo.altitude));
            if ((altitude - _apogeeinfo.altitude) < alt_threshold) { // if we have passed apogee and now descending, could put a bound on here too
                _apogeeinfo.reached = true;
                // log apogee time and altitude
            }
        }
    }
    prevCheckApogeeTime = millis();
    
    return _apogeeinfo;
};

// Moving average sums
// Sums for t, t^2, t^3, t^4 and a, a*t, a*t^2
void ApogeeDetect::updateSums(float oldTime, float newTime, float oldAlt, float newAlt) {
    float newTime_2 = newTime * newTime;
    float newTime_3 = newTime_2 * newTime;
    float newTime_4 = newTime_3 * newTime;

    float oldTime_2 = oldTime * oldTime;
    float oldTime_3 = oldTime_2 * oldTime;
    float oldTime_4 = oldTime_3 * oldTime;

    sumTime += newTime - oldTime;
    sumTime_2 += newTime_2 - oldTime_2;
    sumTime_3 += newTime_3 - oldTime_3;
    sumTime_4 += newTime_4 - oldTime_4;

    sumAlt += newAlt - oldAlt;

    sumAltTime += newAlt * newTime - oldAlt * oldTime;
    sumAltTime_2 += newAlt * newTime_2 - oldAlt * oldTime_2;

    // _logcontroller.log(std::to_string(sumTime) + "\t" + std::to_string(sumTime) + "\t" + std::to_string(sumTime_2) + "\t" + std::to_string(sumTime_3) + "\t" + std::to_string(sumTime_4) + "\t" + std::to_string(sumAlt) + "\t" +std::to_string(sumAltTime) + "\t" + std::to_string(sumAltTime_2) + "\t" );
};

/*Create a matrix, three simulatneous equations */
void ApogeeDetect::quadraticFit(float oldTime, float newTime, float oldAlt, float newAlt) {
    updateSums(oldTime, newTime, oldAlt, newAlt);  // update sums with new values and remove old values
    // re populate the arrays
    A << float(altitude_array.size()), sumTime, sumTime_2,
        sumTime, sumTime_2, sumTime_3,
        sumTime_2, sumTime_3, sumTime_4;

    std::stringstream a;
    a << A;
    // _logcontroller.log(a.str());

    b << sumAlt, sumAltTime, sumAltTime_2;

    std::stringstream b_str;
    b_str << b;
    // _logcontroller.log(b_str.str());
    // solve the system for the coefficents
    coeffs = A.colPivHouseholderQr().solve(b);
}
