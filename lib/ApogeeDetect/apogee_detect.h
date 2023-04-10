#pragma once

#include "AccumulatingRingBuffer.h"
#include <ArduinoEigenDense.h>

struct ApogeeInfo {
    bool reached;
    float alt;
    uint32_t time;  // time in ms
};

class ApogeeDetect {
   public:
    /**
     * @brief Construct a new Apogee Detect object
     *
     * @param sample_time  in millis
     */
    ApogeeDetect(uint32_t time, uint16_t sample_time);
    /**
     * @brief
     *
     * @param time
     * @param alt expects up + be careful!
     * @return const ApogeeInfo&
     */
    const ApogeeInfo& check_apogee(uint32_t time, float alt);

   private:
    static constexpr int arrayLen = 100;
    AccumulatingRingBuffer<arrayLen> buf;

    const uint16_t sample_time;
    uint32_t prev_check_apogee_time = 0;
    uint32_t initial_entry_time = 0;

    void quadraticFit();

    Eigen::Matrix3f A;
    Eigen::Vector3f b;
    Eigen::Vector3f coeffs;

    static constexpr float alt_threshold = 0;  // threshold to detect altitude descent
    static constexpr float alt_min = 100;      // Minimum altitude (m) before apogee detection algorithm works

    ApogeeInfo apogee_info;  // create the structure for ApogeeInfo
};
