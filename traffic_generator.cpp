#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>

// Function to generate random alphanumeric vehicle ID
std::string generateVehicleID() {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string id;
    for (int i = 0; i < 8; ++i) {
        id += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return id;
}

int main() {
    srand(time(nullptr));

    // Lane files
    std::string laneFiles[] = {"lanea.txt", "laneb.txt", "lanec.txt", "laned.txt"};
    std::string laneNames[] = {"AL2", "BL2", "CL2", "DL2"};

    int vehicleCount = 0;

    // Run for a specified time or indefinitely
    for (int i = 0; i < 100; ++i) {  // Generate 100 vehicles
        int delay = 1 + rand() % 1;
        std::this_thread::sleep_for(std::chrono::seconds(delay));

        // Select random lane
        int laneIndex = rand() % 4;

        // Generate vehicle
        std::string vehicleID = generateVehicleID();
        time_t currentTime = time(nullptr);

        // Write to lane file
        std::ofstream outFile(laneFiles[laneIndex], std::ios::app);
        if (outFile.is_open()) {
            outFile << vehicleID << "," << currentTime << "," << laneNames[laneIndex] << std::endl;
            outFile.close();

            vehicleCount++;
            std::cout << "Vehicle " << vehicleID << " generated on " << laneNames[laneIndex]
                      << " (Total: " << vehicleCount << ")" << std::endl;
        } else {
            std::cerr << "Error: Could not open file " << laneFiles[laneIndex] << std::endl;
        }
    }

    return 0;
}
