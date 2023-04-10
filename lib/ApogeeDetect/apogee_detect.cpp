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


template <size_t LEN>
AccumulatingRingBuffer<LEN>::AccumulatingRingBuffer() : acc{0, 0, 0, 0, 0, 0, 0, 0} {}

template <size_t LEN>
void AccumulatingRingBuffer<LEN>::push(uint32_t new_time, float new_alt) {
    uint32_t old_time = time_array.pop_push_back(new_time);
    float old_alt = altitude_array.pop_push_back(new_alt);

    float new_time_2 = new_time * new_time;
    float new_time_3 = new_time_2 * new_time;
    float new_time_4 = new_time_3 * new_time;

    float new_alt_time = new_alt * new_time;
    float new_alt_time_2 = new_alt * new_time_2;
   
    float old_time_2 = old_time * old_time;
    float old_time_3 = old_time_2 * old_time;
    float old_time_4 = old_time_3 * old_time;

    float old_alt_time = old_alt * old_time;
    float old_alt_time_2 = old_alt * old_time_2;

    acc.count = static_cast<float>(time_array.size());
    acc.sum_time += new_time - old_time;
    acc.sum_time_2 += new_time_2 - old_time_2;
    acc.sum_time_3 += new_time_3 - old_time_3;
    acc.sum_time_4 += new_time_4 - old_time_4;

    acc.sum_alt += new_alt - old_alt;
    acc.sum_alt_time += new_alt_time - old_alt_time;
    acc.sum_alt_time_2 += new_alt_time_2 - old_alt_time_2;
}