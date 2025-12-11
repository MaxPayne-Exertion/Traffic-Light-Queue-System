#include <fstream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

std::string idgen(){
    const std::string charset =
           "0123456789"
           "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string idval;

    for (int i = 0; i <=8; ++i) {
        // Generate a random index within the bounds of the CHARACTERS string
        int random_index = rand() % charset.length();
        // Append the character at the random index to the result string
        idval+= charset[random_index];
    }

    return idval;
}

int main() {
    srand(time(0));
    const char* files[] = {"laneA.txt", "laneB.txt", "laneC.txt", "laneD.txt"};


       while (true) {
           // Pick random file (0, 1, 2, or 3)
           int random_index = rand() % 4;  // rand() % 4 gives 0, 1, 2, or 3
           const char* filename = files[random_index];
           std::string vehicle_id = idgen();

           // Open and write to the file
           std::ofstream file(filename, std::ios::app);
           if (file.is_open()) {
               file << "V " << vehicle_id << "\n";
           }

           // Wait 400-1200 milliseconds (0.4 to 1.2 seconds)
           int delay = 400 + (rand() % 800);  // rand() % 800 gives 0-799
           std::this_thread::sleep_for(std::chrono::milliseconds(delay));
       }

       return 0;
   }
