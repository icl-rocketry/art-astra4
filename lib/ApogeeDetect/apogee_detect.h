#pragma once

#include "AccumulatingRingBuffer.h"
#include <ArduinoEigenDense.h>

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
    ApogeeDetect(uint32_t time, uint16_t sampleTime);
    /**
     * @brief
     *
     * @param time
     * @param alt expects up + be careful!
     * @return const ApogeeInfo&
     */
    const ApogeeInfo& checkApogee(uint32_t time, float altitude);

   private:
    static constexpr int arrayLen = 100;
    AccumulatingRingBuffer<arrayLen> buf;

    const uint16_t _sampleTime;
    uint32_t prev_check_apogee_time = 0;
    uint32_t initial_entry_time = 0;

    void quadraticFit();

    Eigen::Matrix3f A;
    Eigen::Vector3f b;
    Eigen::Vector3f coeffs;

    static constexpr float alt_threshold = 0;  // threshold to detect altitude descent
    static constexpr float alt_min = 100;      // Minimum altitude (m) before apogee detection algorithm works

    ApogeeInfo _apogeeinfo;  // create the structure for ApogeeInfo
};
