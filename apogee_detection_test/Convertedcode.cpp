#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <Eigen/Dense>

int main() {
    // Variables 
    const double refpressure = 101728.25;
    float maxthrustaltitude = 220;
    float ag1 = 300; // Apogee guess 1.
    float ag2 = 350; // Apogee guess 2. 
    float min_triggeralt = 10;
    float flighttime = 45; // after motor burnout
    double val;
    int MTI;
    double MTAT;
    double referenceTime;
    vector<double> Ts;
    vector<double> As; 
    Eigen::Matrix3d y;
    Eigen::Vector3d z;
    Eigen::Vector2d coeffs;
    Eigen::Vector2d velcoeffs;
    double velcoeffs_1;
    double velcoeffs_2;
    float tguess;
    


    std::ifstream file("astra2_full_testdata.txt");
    std::vector<std::vector<double>> data;
    std::vector<std::vector<double>> availabledata; // Defining empty arrays.
    std::vector<std::vector<double>> launchData; // Defining empty arrays

    if (file.is_open()) {
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

      auto it = std::find_if(availabledata.begin(), availabledata.end(), 
        [min_triggeralt](double value) { 
            return value > min_triggeralt && value >= 5000 && value <= 6000; 
        });

        if (it != availabledata.end()) {
            val = *it;
        } 

        for (int i = val; i < availabledata.size(); i++ ){
            launchData.push_back({ availabledata[i][0], availabledata[i][2] });
        }

        // Checking if the max thrust altitude has been reached
        // the time at which this happens is called MTAT
        // Index for that time is called MTI - THERE MIGHT BE A PROBLEM HERE IN THE INDEXING!!!
        if (availabledata[i][2] > maxthrustaltitude) {
            MTI = 0;
            for (int j = 0; j < launchData.size(); j++) {
                if (launchData[j][1] > maxthrustaltitude) {
                    MTI = j;
                    break;
                }
            }

            if (MTI != 0) {
                MTAT = launchData[MTI][0];  
            }
        }

        int launchDataSize = launchData.size();
        if (MTI >= 0 && MTI < launchDataSize) {
            referenceTime = launchData[0][0];
            for (int i = MTI; i < launchDataSize; i++) {
                Ts.push_back(launchData[i][0] - referenceTime);
                As.push_back(launchData[i][1]);
            }
        }



        // Calculate y matrix
        y << std::pow(Ts.sum(), 4), std::pow(Ts.sum(), 3), std::pow(Ts.sum(), 2),
        std::pow(Ts.sum(), 3), std::pow(Ts.sum(), 2), Ts.sum(),
        std::pow(Ts.sum(), 2), Ts.sum(), Ts.size();

        // Calculate z vector
        z << (As * (Ts.array().pow(2))).sum(), (As * Ts.array()).sum(), As.sum();

        // Calculate coeffs vector using least squares
        coeffs = y.colPivHouseholderQr().solve(z);

        // Calculate velcoeffs
        velcoeffs_1 = 2 * coeffs(0); // we've set these to be doubles, but they may not be doubles?? please check. 
        velcoeffs_2 = coeffs(1);  //''

        ag1 = ag2;
        tguess = -velcoeffs(2)/velcoeffs(1);
        ag2 = coeffs(1)*tguess^2 + coeffs(2)*tguess + coeffs(3);

    // We've stopped converting into c++, on line 96 (in the matlab version, this is where the video related things start)

(;    }











