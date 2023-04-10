

#include "apogee_detect.h"

#include <Arduino.h>

#include <iostream>
#include <sstream>
#include <vector>

ApogeeDetect::ApogeeDetect(uint32_t time, uint16_t sampleTime) : _sampleTime(sampleTime),
                                                                 initial_entry_time(time),
                                                                 _apogeeinfo({false, 0, 0}) {
}

// Function to update flight data values and return data for apogee prediction
const ApogeeInfo &ApogeeDetect::checkApogee(uint32_t time, float alt) {
    // If this is called too fast, don't do anything
    if (time - prev_check_apogee_time <= _sampleTime) {
        return _apogeeinfo;
    }

    uint32_t timeSinceEntry = time - initial_entry_time;

    buf.push(timeSinceEntry, alt);

    // Apogee detection:
    if (!(_apogeeinfo.reached)) {
        // Fit a quadratic to the data
        quadraticFit();

        _apogeeinfo.time = (-coeffs(1) / (2 * coeffs(2))) + initial_entry_time;  // maximum from polyinomial using derivative

        if ((time >= _apogeeinfo.time) && (coeffs(2) < 0) && (time > 0) && (alt > alt_min)) {
            _apogeeinfo.altitude = coeffs(0) - (std::pow(coeffs(1), 2) / (4 * coeffs(2)));
            
            if ((alt - _apogeeinfo.altitude) < alt_threshold) { // if we have passed apogee and now descending, could put a bound on here too
                _apogeeinfo.reached = true;
            }
        }
    }
    prev_check_apogee_time = time;
    
    return _apogeeinfo;
};

/*Create a matrix, three simultaneous equations */
void ApogeeDetect::quadraticFit() {
    const Accumulated& acc = buf.get_accumulated();
    
    // re populate the arrays
    A << acc.count,      acc.sum_time,   acc.sum_time_2,
         acc.sum_time,   acc.sum_time_2, acc.sum_time_3,
         acc.sum_time_2, acc.sum_time_3, acc.sum_time_4;

    b << acc.sum_alt, acc.sum_alt_time, acc.sum_alt_time_2;

    // solve the system for the coefficents
    coeffs = A.colPivHouseholderQr().solve(b);
}
