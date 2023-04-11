#pragma once

#include <stdint.h>
#include <queue>

#ifdef BOARD
#include <ArduinoEigenDense.h>
#else
#include <Eigen/Dense>

#ifdef DEBUG
#include <iostream>
#endif
#endif

template <typename T, size_t LEN>
class RingBuf;

// TODO: Rename this
struct Accumulated {
    float count;      // Σ(t^0) = Σ1
    float sum_time;   // Σt
    float sum_time_2; // Σ(t^2)
    float sum_time_3; // Σ(t^3)
    float sum_time_4; // Σ(t^4)

    float sum_alt;        // Σa
    float sum_alt_time;   // Σat
    float sum_alt_time_2; // Σ(at^2)
};

/**
 * @brief Ring buffer like data structure that maintains sums for apogee detection
 * 
 * @tparam LEN size_t
 */
template <size_t LEN>
class AccumulatingRingBuffer {
public:
    AccumulatingRingBuffer();

    void push(uint32_t new_time, float new_alt);
    
    const Accumulated& get_accumulated() const {
        return acc;
    }

private:
    Accumulated acc;

    RingBuf<float, LEN> time_array;
    RingBuf<float, LEN> altitude_array;

};


/**
 * @brief Simple ringbuf implementation based on std::queue
 *
 * @tparam T
 * @tparam LEN size_t
 */
template <typename T, size_t LEN>
class RingBuf : private std::queue<T> {
public:
    /**
     * @brief inserts a new value at the front and returns the removed value.
     * Returns zero if len != maxLen
     *
     * @param val
     * @return T
     */
    T pop_push_back(T val) {
        // push new element on queue
        this->push(val);

        const size_t curr_size = this->size();

        if (curr_size == LEN + 1) {
            T lastVal = this->front();
            this->pop();
            return lastVal;
        } else if (curr_size < LEN + 1) {
            return 0;
        } else if (curr_size > LEN + 1) {
            throw std::runtime_error("RingBuf size exceeded!");
        }
        return 0;
    };

    using std::queue<T>::size;

private:
    static constexpr size_t maxLen = LEN;
};

/**
 * @brief Predicts apogee given sampled times and pressures
 * @remark This class should give multiple predictions over time
*/
class ApogeePredictor {
public:
    /**
     * @param initial_entry_time in millis
    */
    ApogeePredictor(uint32_t initial_entry_time);

    /**
     * @brief Feed a time and altitude reading into the predictor
     * @param time in millis
     * @param altitude in meters
     * @returns predicted apogee altitude
    */
    float feed(uint32_t time, float alt);

    /**
     * @brief Get the absolute time of apogee
     * @returns the absolute time of apogee
    */
    uint32_t get_apogee_time() const;

private:
    static constexpr int arrayLen = 100;
    AccumulatingRingBuffer<arrayLen> buf;

    Eigen::Matrix3f A;
    Eigen::Vector3f b;
    Eigen::Vector3f coeffs;

    const uint32_t initial_entry_time; // time in millis that this was entered
};

class ApogeeDetector {
   public:
    /**
     * @brief Construct a new Apogee Detector object
     *
     * @param sample_time  in millis
     */
    ApogeeDetector(uint32_t initial_entry_time, uint16_t sample_time);
    /**
     * @brief
     * @remark Should only be called until apogee has been reached
     * @param time
     * @param alt expects up + be careful!
     * @return whether apogee has been reached
     */
    bool detect(uint32_t time, float alt);

   private:
    ApogeePredictor predictor;
    uint32_t prev_check_apogee_time = 0;
    const uint16_t sample_time;

    float prev_apogee;

    static constexpr float alt_threshold = 10;  // threshold to detect altitude descent
    static constexpr float alt_min = 10;      // Minimum altitude (m) before apogee detection algorithm works
    
    bool apogee_found;
    uint32_t apogee_time;
};