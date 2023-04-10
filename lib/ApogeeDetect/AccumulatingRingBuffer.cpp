#include "AccumulatingRingBuffer.h"

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