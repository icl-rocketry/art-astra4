#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <vector>
#include <exception>
#include "../lib/ApogeeDetect/apogee_detect.h"

const float refpressure = 101728.25;

float pressure_to_altitude(const float pressure) {
    return 44330 * (1 - std::pow(pressure/refpressure, 1/5.255));
}
 
int main(int argc, char **argv) {
    if (argc < 2) {
        throw std::invalid_argument("No data file specified");
    }

    std::ifstream data(argv[1]);
    
    std::vector<uint32_t> times;
    std::vector<float> pressures;
    
    std::string line;
    std::string word; 
    while (std::getline(data, line)) {
        std::stringstream ss(line);

        std::getline(ss, word, ',');
        times.push_back(std::stoul(word));
        std::getline(ss, word, ',');
        pressures.push_back(std::stof(word));
    }

    data.close();

    ApogeeDetect detector(times.at(0), 20);

    uint32_t time;
    float pressure;

    for (int i = 0; i < times.size(); i++) {
        if (pressure == pressures[i]) continue;

        time = times[i];
        pressure = pressures[i];

        const auto& info = detector.check_apogee(time, pressure_to_altitude(pressure));

        std::cout << info.alt << "," << info.reached << "," << info.time << std::endl;
    }

    return 0;
}
