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
    
    std::vector<uint32_t> times;  // declares 2 empty vectors
    std::vector<float> pressures;
    
    std::string line;   // declares 2 strings
    std::string word; 
    while (std::getline(data, line)) {   // processes a comma seperated values on to new line
        std::stringstream ss(line);    // ss is an object - each line is read into it

        std::getline(ss, word, ',');
        times.push_back(std::stoul(word));    // adding to time vector
        std::getline(ss, word, ',');
        pressures.push_back(std::stof(word));  // adding to pressure vector
    }

    data.close();

    ApogeeDetector detector(times.at(0), 20);  // creating a new class called detector - 1st element of time vector and a value 20 (?)

    uint32_t time;
    float pressure;   // appogee pressure is supposed to be very low but not exactly 0!!!!!!


    // iterating over the times vector 
    for (int i = 0; i < times.size(); i++) {   
        if (pressure == pressures[i]) continue;   // check if current pressure value is equal to pressures[i] - pressures[i] is the pressure at 

        time = times[i];
        pressure = pressures[i];

        bool reached = detector.detect(time, pressure);

        std::cout << reached << ", " << pressure << ", " << time << std::endl;   // outputting apogee time and pressure
    }

    return 0;
}


//possible corrections
//Incorrect Data: It's possible that the data being read from the file does not represent the actual altitude or pressure values correctly. If the data is incorrect or improperly formatted, it could lead to incorrect detection of apogee.
//Incorrect Implementation: The logic within the ApogeeDetector class might not be properly implemented to detect apogee based on altitude and pressure values. There could be a bug or error in the implementation that causes incorrect detection.
//Incorrect Configuration: The parameters used to configure the ApogeeDetector instance, such as the initial time (times.at(0)) or the threshold value (20), might not be set correctly for the given data. Adjusting these parameters according to the specific context and data characteristics could be necessary to achieve accurate apogee detection.