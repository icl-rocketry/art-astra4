#!/bin/bash

# Check if eigen exists and if not, unzip it
if ! [[ -d "apogee_detection_test/eigen-3.4.0/" ]]; then
    echo "Unzipping eigen"
    (
        cd "./apogee_detection_test" || (echo "WRONG FOLDER" && exit 1)
        unzip "eigen-3.4.0.zip"
    )
fi

g++ -I apogee_detection_test/eigen-3.4.0 apogee_detection_test/main.cpp lib/ApogeeDetect/apogee_detect.cpp