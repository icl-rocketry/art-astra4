#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include "../lib/ApogeeDetect/ApogeeDetector.h"
 
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

    ApogeeDetector detector(times.at(0), 20);

    uint32_t time;
    float pressure;

    for (int i = 0; i < times.size(); i++) {
        if (pressure == pressures[i]) continue;

        time = times[i];
        pressure = pressures[i];

        bool reached = detector.detect(time, pressure);

        std::cout << reached << ", " << pressure << ", " << time << std::endl;
    }

    return 0;
}
