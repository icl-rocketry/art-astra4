#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <vector>
#include <exception>
#include "../lib/ApogeeDetect/apogee_detect.h"
 
int main(int argc, char **argv) {
    std::ifstream data("./Code/astra2_air.txt");
    
    std::vector<uint32_t> times;
    std::vector<float> pressures;
    
    std::string line;
    std::string word; 
    while (std::getline(data, line)) {
        std::stringstream ss(line);

        std::getline(ss, word, ',');
        pressures.push_back(std::stof(word));
        std::getline(ss, word, ',');
        times.push_back(std::stoul(word));
    }

    data.close();

    ApogeeDetect detector(times.at(0), 20);

    for (int i = 0; i < times.size(); i++) {
        uint32_t time = times[i];
        float pressure = pressures[i];

        const auto& info = detector.check_apogee(time, pressure);

        std::cout << info.alt << "," << info.reached << "," << info.time << std::endl;
    }

    return 0;
}
