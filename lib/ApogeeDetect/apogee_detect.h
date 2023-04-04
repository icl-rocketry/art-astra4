#pragma once

#include <ArduinoEigenDense.h>

#include <algorithm>
#include <array>
#include <queue>
#include <vector>

/**
 * @brief Simple ringbuf implementation based on std::queue
 *
 * @tparam T
 * @tparam LEN
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

struct ApogeeInfo {
    bool reached;
    float altitude;
    uint32_t time;  // time in ms
};

class ApogeeDetect {
   public:
    /**
     * @brief Construct a new Apogee Detect object
     *
     * @param sampleTime  in millis
     */
    ApogeeDetect(uint16_t sampleTime);
    /**
     * @brief
     *
     * @param altitude expects up + be careful!
     * @param velocity
     * @param time
     * @return const ApogeeInfo&
     */
    const ApogeeInfo& checkApogee(float altitude, float velocity, uint32_t time);  // create function in the memory address of the structure to estimate the apogee

   private:
    static constexpr int arrayLen = 100;

    const uint16_t _sampleTime;
    uint32_t prevCheckApogeeTime = 0;
    uint32_t initialEntryTime = 0;

    RingBuf<uint32_t, arrayLen> time_array;  // create arrays to store recent flight history for apogee approximation
    RingBuf<float, arrayLen> altitude_array;

    /**
     * @brief Returns the coefficents of a 2nd order fitted polynomial
     *
     * @param time_array
     * @param altitude_array
     * @return std::array<float,2>
     */
    void quadraticFit(float oldTime, float newTime, float oldAlt, float newAlt);
    // sums for fitting polynomial
    float sumTime;
    float sumTime_2;
    float sumTime_3;
    float sumTime_4;
    float sumAlt;
    float sumAltTime;
    float sumAltTime_2;

    void updateSums(float oldTime, float newTime, float oldAlt, float newAlt);

    Eigen::Matrix3f A;
    Eigen::Vector3f b;
    Eigen::Vector3f coeffs;

    uint32_t prevMachLockTime;
    static constexpr float mlock_speed = 30;  // value chosen thru tuning -> depends on filter performance
    static constexpr int mlock_time = 1000;   // lockout time in milliseconds

    static constexpr float alt_threshold = 0;  // threshold to detect altitude descent
    static constexpr float alt_min = 100;      // Minimum altitude (m) before apogee detection algorithm works
    bool mlock;                               // Mach lockout activated by default on launch

    ApogeeInfo _apogeeinfo;  // create the structure for ApogeeInfo
};
