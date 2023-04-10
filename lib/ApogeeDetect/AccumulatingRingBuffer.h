#pragma once

#include <stdint.h>
#include <queue>

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

    RingBuf<uint32_t, LEN> time_array;
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