#include <fstream>

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
           auto now = std::chrono::system_clock::now();
           auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
           auto value = now_ms.time_since_epoch();
           long spawn_time = value.count();

           // Open and write to the file
           std::ofstream file(filename, std::ios::app);
           if (file.is_open()) {
               file <<vehicle_id<<" "<<spawn_time<< "\n";
           }
       }

       return 0;
   }
