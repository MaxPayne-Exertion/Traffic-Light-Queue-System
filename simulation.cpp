#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#include "queue.cpp"

#define LANES 4

class Simulation {
private:
    Lane lanes[LANES];

public:
    Simulation() {
        lanes[0] = Lane("AL2", true);   // priority lane
        lanes[1] = Lane("BL2");
        lanes[2] = Lane("CL2");
        lanes[3] = Lane("DL2");
    }

    // Read vehicle data from text files
    void readFiles() {
        string files[LANES] = {
            "laneA.txt",
            "laneB.txt",
            "laneC.txt",
            "laneD.txt"
        };

        for (int i = 0; i < LANES; i++) {
            ifstream fin(files[i]);
            if (!fin) continue;

            int id, time;
            while (fin >> id >> time) {
                Vehicle v(id, time);
                lanes[i].vehicleQ.enqueue(v.id);

            }
            fin.close();

            // Clear file after reading
            ofstream clear(files[i], ios::trunc);
            clear.close();
        }
    }

    void updatePriorities() {
        for (int i = 0; i < LANES; i++) {
            lanes[i].isPriority = lanes[i].vehicleQ.size();
        }

        // Priority rule for AL2
        if (lanes[0].vehicleQ.size() > 10)
            lanes[0].isPriority = 100;
        else if (lanes[0].vehicleQ.size() < 5)
            lanes[0].isPriority = lanes[0].vehicleQ.size();
    }

    int selectLane() {
        int idx = 0;
        for (int i = 1; i < LANES; i++) {
            if (lanes[i].isPriority > lanes[idx].isPriority)
                idx = i;
        }
        return idx;
    }

    int vehiclesToServe() {
        int sum = 0;
        for (int i = 1; i < LANES; i++)
            sum += lanes[i].vehicleQ.size();

        int v = sum / 3;
        return (v < 1) ? 1 : v;
    }

    void serve() {
        updatePriorities();
        int laneIndex = selectLane();
        int count = vehiclesToServe();

        cout << "\nGREEN LIGHT: " << lanes[laneIndex].name << endl;

        for (int i = 0; i < count; i++) {
            Vehicle vid = lanes[laneIndex].vehicleQ.dequeue();

            cout << "Vehicle " << vid.id << " passed\n";
        }
    }

    void display() {
        cout << "\n--- Lane Status ---\n";
        for (int i = 0; i < LANES; i++) {
            cout << lanes[i].name
                 << " | Vehicles: " << lanes[i].vehicleQ.size()
                 << " | Priority: " << lanes[i].isPriority << endl;
        }
    }
};


int main(){
    Simulation sim;

    while (true) {
        sim.readFiles();
        sim.display();
        sim.serve();

        cout << "\n------------------------\n";
        cin.get();
    }
    return 0;
}
