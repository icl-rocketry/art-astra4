#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

int main() {
    // Variables 
    const double refpressure = 101728.25;
    float maxthrustaltitude = 220;
    float ag1 = 300; // Apogee guess 1.
    float ag2 = 350; // Apogee guess 2. 
    float min_triggeralt = 10;
    float flighttime = 45; // after motor burnout


    std::ifstream file("astra2_full_testdata.txt");
    std::vector<std::vector<double>> data;
    std::vector<std::vector<double>> availabledata; // Defining empty arrays.

    std::file.is_open() {
        double value;
        while (file >> value) {
            std::vector<double> row;
            row.push_back(value);
            data.push_back(row);
        }
        file.close();
    } 
    
    else {
        std::cout << "Failed to open the file." << std::endl;
        return 1;
    }

    for (int i = 5000; i <= 6000; i++) {
        // Adding each time datapoint onto the end of all data array
    availabledata.push_back(std::vector<double>());

    // Assigning values to the availabledata array
    availabledata[i][0] = data[i][0]; // data point
    availabledata[i][1] = data[i][1]; // pressure point
    availabledata[i][2] = 44330 * (1 - pow(availabledata[i][1] / refpressure, 1.0 / 5.255));

    // Checking if the altitude is above 10m. 
    // if availabledata(i,3) > min_triggeralt

    }
}
