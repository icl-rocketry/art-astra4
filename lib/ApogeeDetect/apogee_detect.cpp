

#include "apogee_detect.h"

ApogeeDetect::ApogeeDetect(uint32_t time, uint16_t sample_time) : sample_time(sample_time),
                                                                 initial_entry_time(time),
                                                                 apogee_info({false, 0, 0}) {
}

// Function to update flight data values and return data for apogee prediction
const ApogeeInfo &ApogeeDetect::check_apogee(uint32_t time, float alt) {
    // If this is called too fast, don't do anything
    if (time - prev_check_apogee_time <= sample_time) {
        return apogee_info;
    }

    uint32_t time_since_entry = time - initial_entry_time;

    buf.push(time_since_entry, alt);

    // Apogee detection:
    if (!(apogee_info.reached)) {
        // Fit a quadratic to the data
        quadraticFit();

        apogee_info.time = (-coeffs(1) / (2 * coeffs(2))) + initial_entry_time;  // maximum from polyinomial using derivative

        if ((time >= apogee_info.time) && (coeffs(2) < 0) && (time > 0) && (alt > alt_min)) {
            apogee_info.alt = coeffs(0) - (std::pow(coeffs(1), 2) / (4 * coeffs(2)));
            
            if ((alt - apogee_info.alt) < alt_threshold) { // if we have passed apogee and now descending, could put a bound on here too
                apogee_info.reached = true;
            }
        }
    }
    prev_check_apogee_time = time;
    
    return apogee_info;
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
