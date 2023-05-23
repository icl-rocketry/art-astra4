#include "ApogeeDetector.h"
#include <iostream>

ApogeePredictor::ApogeePredictor(uint32_t initial_entry_time) : initial_entry_time(initial_entry_time) {}

float ApogeePredictor::predict(uint32_t time, float alt) {
    uint32_t time_since_entry = time - initial_entry_time;

    buf.push(static_cast<float>(time_since_entry) / 1000, alt);

    const Accumulated& acc = buf.get_accumulated();
    
    // re populate the arrays
    A << acc.count,      acc.sum_time,   acc.sum_time_2,
         acc.sum_time,   acc.sum_time_2, acc.sum_time_3,
         acc.sum_time_2, acc.sum_time_3, acc.sum_time_4;

    b << acc.sum_alt, acc.sum_alt_time, acc.sum_alt_time_2;

    // solve the system for the coefficents
    coeffs = A.colPivHouseholderQr().solve(b);

    return coeffs(0) - (std::pow(coeffs(1), 2) / (4 * coeffs(2))); // c - b^2/4a (aka the highest point)
}

uint32_t ApogeePredictor::get_apogee_time() const {
    float relative_apogee_time = 1000 * (-coeffs(1) / (2 * coeffs(2)));
    return static_cast<uint32_t>(relative_apogee_time) + initial_entry_time;  // maximum from polyinomial using derivative
}

ApogeeDetector::ApogeeDetector(uint32_t initial_entry_time, uint16_t min_sample_time) : min_sample_time(min_sample_time), predictor(initial_entry_time) {}

bool ApogeeDetector::detect(uint32_t time, float pressure) {
    if (apogee_found) {
        return time >= apogee_time;
    }

    // If this is called too fast, don't do anything
    if (time - prev_check_apogee_time <= min_sample_time) {
        return false;
    }
    prev_check_apogee_time = time;

    float apogee = predictor.predict(time, pressure_to_altitude(pressure));

    #ifdef DEBUG
        std::cout << "Apogee: "        << apogee 
                  << ", Prev_Apogee: " << prev_apogee
                  << ", >AltMin: "     << (apogee > alt_min)
                  << ", diff: "        << abs(apogee - prev_apogee)
                  << ", Time: "        << predictor.get_apogee_time()
                  << std::endl;
    #endif
    
    // We've found apogee iff we're above alt_min meters and our apogee guesses have converged 
    apogee_found = apogee > alt_min && abs(apogee - prev_apogee) < alt_threshold;
    prev_apogee = apogee;

    if (apogee_found) {
        apogee_time = predictor.get_apogee_time();
    }
    
    return apogee_found && time >= apogee_time;
};

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