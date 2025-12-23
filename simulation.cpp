#include <iostream>
#include <fstream>
#include <string>
#include <chrono>


using namespace std;
#include "queue.cpp"

#define LANES 4

class Simulation {
private:
    Lane lanes[LANES];
    bool lights[LANES];  // true = green, false = red

public:
    Simulation() {
        lanes[0] = Lane("AL2", 0);  // Priority lane
        lanes[1] = Lane("BL2", 0);
        lanes[2] = Lane("CL2", 0);
        lanes[3] = Lane("DL2", 0);

       // All lights start as red
        for (int i = 0; i < LANES; i++) {
            lights[i] = false;
        }
        // Seed for random operations
        srand(time(0));
    }

    Lane& getLane(int i) { return lanes[i]; }

    // Read vehicle data from text files
    void readFiles() {
        string files[LANES] = {"laneA.txt", "laneB.txt", "laneC.txt", "laneD.txt"};

        for (int i = 0; i < LANES; i++) {
            ifstream fin(files[i]);
            if (!fin) {
                // Create file if it doesn't exist
                ofstream create(files[i]);
                create.close();
                continue;
            }

            string id;
            long spawn_time;
            int vehicles_read = 0;

            while (fin >> id >> spawn_time) {
                Vehicle v(id, spawn_time);
                lanes[i].vehicleQ.enqueue(v);
                vehicles_read++;
            }
            fin.close();

            if (vehicles_read > 0) {
                cout << "Read " << vehicles_read << " vehicles from " << files[i] << endl;

                // Clear file after reading
                ofstream clear(files[i], ios::trunc);
                clear.close();
            }
        }
    }
    void updatePriorities() {
        // Reset all priorities to queue sizes
        for (int i = 0; i < LANES; i++) {
            lanes[i].isPriority = lanes[i].vehicleQ.size();
        }

        // Special priority rule for AL2 (lane[0]) according to assignment
        if (lanes[0].vehicleQ.size() > 10) {
            lanes[0].isPriority = 100;  // Highest priority when >10 vehicles
            cout << "AL2 has HIGH PRIORITY (>10 vehicles)" << endl;
        } else if (lanes[0].vehicleQ.size() < 5) {
            // AL2 becomes normal lane when < 5 vehicles
            lanes[0].isPriority = lanes[0].vehicleQ.size();
            cout << "AL2 is NORMAL LANE (<5 vehicles)" << endl;
        }
        // If between 5-10 vehicles, keep current priority (which is queue size)
    }

    int selectLane() {
        // Find lane with highest priority
        int selectedLane = 0;
        int maxPriority = lanes[0].isPriority;

        for (int i = 1; i < LANES; i++) {
            if (lanes[i].isPriority > maxPriority) {
                maxPriority = lanes[i].isPriority;
                selectedLane = i;
            }
        }

        // If priority lane (AL2) has 100 priority, it should be selected
        if (lanes[0].isPriority == 100) {
            selectedLane = 0;
        }

        cout << "Selected lane: " << lanes[selectedLane].name
             << " with priority: " << maxPriority << endl;
        return selectedLane;
    }


    int vehiclesToServe(int currentLane) {
        if (currentLane == 0) {  // AL2 (priority lane)
            // When priority lane is active, serve all vehicles
            return lanes[0].vehicleQ.size();
        } else {
            // Normal lanes: average of other normal lanes
            // According to assignment: n = 3 (BL2, CL2, DL2)
            int sum = 0;
            int count = 0;

            // Sum vehicles in normal lanes (excluding the current lane)
            for (int i = 1; i < LANES; i++) {
                if (i != currentLane) {
                    sum += lanes[i].vehicleQ.size();
                    count++;
                }
            }

            if (count == 0) return 1;

            int vehicles_to_serve = sum / count;

            // Ensure at least 1 vehicle is served
            if (vehicles_to_serve < 1) {
                vehicles_to_serve = 1;
            }

            // Don't serve more than available in current lane
            int available = lanes[currentLane].vehicleQ.size();
            if (vehicles_to_serve > available) {
                vehicles_to_serve = available;
            }

            return vehicles_to_serve;
        }
    }
     void updateLights(int greenLane) {
         // Set all lights to red first
        for (int i = 0; i < LANES; i++) {
            lights[i] = false;
        }

        // Set selected lane to green
        lights[greenLane] = true;
    }

    void serve() {
        cout << "\n=== SERVING VEHICLES ===" << endl;

        // Update lane priorities
        updatePriorities();

        // Select which lane gets green light
        int laneIndex = selectLane();

        // Update traffic lights
        updateLights(laneIndex);

        // Calculate how many vehicles to serve
        int vehiclesToProcess = vehiclesToServe(laneIndex);

        // Get current time for wait calculation
        auto now = chrono::system_clock::now();
        auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
        long current_time = now_ms.time_since_epoch().count();

        cout << "\nLane " << lanes[laneIndex].name << " is GREEN" << endl;
        cout << "Processing " << vehiclesToProcess << " vehicles" << endl;
        cout << "------------------------" << endl;

        // Serve vehicles from selected lane
        int vehiclesServed = 0;
        for (int i = 0; i < vehiclesToProcess; i++) {
            if (!lanes[laneIndex].vehicleQ.isEmpty()) {
                Vehicle v = lanes[laneIndex].vehicleQ.dequeue();
                long wait_time = current_time - v.spawntime;

                cout << "Vehicle " << v.id
                     << " passed | Wait time: " << wait_time << "ms" << endl;
                vehiclesServed++;
            }
        }

        // Show results
        cout << "------------------------" << endl;
        cout << "Served " << vehiclesServed << " vehicles from " << lanes[laneIndex].name << endl;
        cout << "Remaining in " << lanes[laneIndex].name << ": "
             << lanes[laneIndex].vehicleQ.size() << " vehicles" << endl;
    }

    void display() {
        cout << "\n=== CURRENT LANE STATUS ===" << endl;
        cout << "Lane\tVehicles\tPriority\tLight" << endl;
        cout << "----\t--------\t--------\t-----" << endl;

        int totalVehicles = 0;
        for (int i = 0; i < LANES; i++) {
            int vehicles = lanes[i].vehicleQ.size();
            totalVehicles += vehicles;

            cout << lanes[i].name << "\t"
                 << vehicles << "\t\t"
                 << lanes[i].isPriority << "\t\t"
                 << (lights[i] ? "GREEN" : "RED") << endl;
        }

        cout << "\nTotal vehicles waiting: " << totalVehicles << endl;
    }
    // Add a method to check if any lane has vehicles
    bool hasVehicles() {
        for (int i = 0; i < LANES; i++) {
            if (!lanes[i].vehicleQ.isEmpty()) {
                return true;
            }
        }
        return false;
    }
};
