#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;
#include "queue.cpp"

#define LANES 4
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define ROAD_WIDTH 300
#define LANE_WIDTH 80

class TrafficSimulator {
private:
    Lane lanes[LANES];
    PriorityQueue lanePQ;
    bool lights[LANES];
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    int currentGreenLane;
    int cycleCount;

public:
    TrafficSimulator() : currentGreenLane(-1), cycleCount(0), font(nullptr) {
        // Initialize lanes
        lanes[0] = Lane("AL2");
        lanes[1] = Lane("BL2");
        lanes[2] = Lane("CL3");
        lanes[3] = Lane("DL4");

        // Initialize priority queue
        for (int i = 0; i < LANES; i++) {
            lanePQ.insert(i, 0);
        }

        // Initialize all lights to red
        for (int i = 0; i < LANES; i++) {
            lights[i] = false;
        }

        // Initialize SDL
        initSDL();
    }

    ~TrafficSimulator() {
        if (font) TTF_CloseFont(font);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }

    void initSDL() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            cerr << "SDL init failed: " << SDL_GetError() << endl;
            exit(1);
        }

        if (TTF_Init() < 0) {
            cerr << "SDL_ttf init failed: " << TTF_GetError() << endl;
            exit(1);
        }

        // Try to load font - try multiple common paths
        const char* fontPaths[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "arial.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "C:/Windows/Fonts/arial.ttf"
        };

        for (const char* path : fontPaths) {
            font = TTF_OpenFont(path, 24);
            if (font) {
                cout << "Loaded font: " << path << endl;
                break;
            }
        }

        if (!font) {
            cerr << "Failed to load font. Using fallback." << endl;
            // Continue without font - we'll use simple drawing
        }

        window = SDL_CreateWindow("Traffic Simulator",
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            cerr << "Window creation failed: " << SDL_GetError() << endl;
            exit(1);
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            cerr << "Renderer creation failed: " << SDL_GetError() << endl;
            exit(1);
        }
    }

    void renderText(const string& text, int x, int y, SDL_Color color = {0, 0, 0, 255}) {
        if (!font) {
            // Fallback: draw a rectangle
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_Rect rect = {x, y, (int)text.length() * 15, 20};
            SDL_RenderFillRect(renderer, &rect);
            return;
        }

        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void renderTextCentered(const string& text, int centerX, int y, SDL_Color color = {0, 0, 0, 255}) {
        if (!font) {
            renderText(text, centerX - (text.length() * 15)/2, y, color);
            return;
        }

        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {centerX - surface->w/2, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void readVehicleData() {
        string files[LANES] = {"laneA.txt", "laneB.txt", "laneC.txt", "laneD.txt"};

        for (int i = 0; i < LANES; i++) {
            ifstream fin(files[i]);
            if (!fin) continue;

            string id;
            long timestamp;

            while (fin >> id >> timestamp) {
                Vehicle v(id, timestamp);
                lanes[i].vehicles.enqueue(v);
            }
            fin.close();

            // Clear file
            ofstream clear(files[i], ios::trunc);
            clear.close();
        }
    }

    void updatePriorities() {
        // AL2 priority rules
        int al2Vehicles = lanes[0].vehicles.size();

        if (al2Vehicles > 10) {
            lanePQ.updatePriority(0, 100);
        } else if (al2Vehicles >= 5) {
            lanePQ.updatePriority(0, 50);
        } else {
            lanePQ.updatePriority(0, al2Vehicles);
        }

        // Other lanes
        for (int i = 1; i < LANES; i++) {
            lanePQ.updatePriority(i, lanes[i].vehicles.size());
        }
    }

    int selectLaneToServe() {
        return lanePQ.extractMax();
    }

    int calculateVehiclesToServe(int laneIndex) {
        if (laneIndex == 0) {
            return lanes[0].vehicles.size();
        }

        int sum = 0;
        int count = 0;

        for (int i = 1; i < LANES; i++) {
            if (i != laneIndex) {
                sum += lanes[i].vehicles.size();
                count++;
            }
        }

        if (count == 0) return 1;
        int avg = sum / count;
        return max(1, min(avg, lanes[laneIndex].vehicles.size()));
    }

    void serveVehicles(int laneIndex) {
        int toServe = calculateVehiclesToServe(laneIndex);

        cout << "\n=== Serving " << lanes[laneIndex].name << " ===" << endl;
        cout << "Vehicles to serve: " << toServe << endl;

        for (int i = 0; i < toServe && !lanes[laneIndex].vehicles.isEmpty(); i++) {
            Vehicle v = lanes[laneIndex].vehicles.dequeue();
            cout << "  Vehicle " << v.id << " passed" << endl;
        }

        cout << "Remaining: " << lanes[laneIndex].vehicles.size() << " vehicles" << endl;
    }

    void setTrafficLights(int greenLane) {
        for (int i = 0; i < LANES; i++) {
            lights[i] = false;
        }

        if (greenLane >= 0) {
            lights[greenLane] = true;
            currentGreenLane = greenLane;
        }
    }

    void render() {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);

        // Draw roads
        drawRoads();

        // Draw lane labels with TTF
        drawLaneLabels();

        // Draw vehicles in queues (centered in lanes)
        drawVehicleQueues();

        // Draw traffic lights
        drawTrafficLights();

        // Draw cycle info
        drawCycleInfo();

        SDL_RenderPresent(renderer);
    }

    void drawRoads() {
        // Vertical road
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_Rect verticalRoad = {WINDOW_WIDTH/2 - ROAD_WIDTH/2, 0, ROAD_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(renderer, &verticalRoad);

        // Horizontal road
        SDL_Rect horizontalRoad = {0, WINDOW_HEIGHT/2 - ROAD_WIDTH/2, WINDOW_WIDTH, ROAD_WIDTH};
        SDL_RenderFillRect(renderer, &horizontalRoad);

        // Lane dividers (white dashed lines)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Calculate lane center positions
        int laneCenters[3];
        for (int i = 0; i < 3; i++) {
            laneCenters[i] = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + (i * LANE_WIDTH) + LANE_WIDTH/2;
        }

        // Vertical road lane markers
        for (int i = 0; i < 3; i++) {
            int laneX = laneCenters[i];

            // Top section (before intersection)
            for (int y = 20; y < WINDOW_HEIGHT/2 - ROAD_WIDTH/2 - 20; y += 40) {
                SDL_RenderDrawLine(renderer, laneX, y, laneX, y + 20);
            }

            // Bottom section (after intersection)
            for (int y = WINDOW_HEIGHT/2 + ROAD_WIDTH/2 + 20; y < WINDOW_HEIGHT - 20; y += 40) {
                SDL_RenderDrawLine(renderer, laneX, y, laneX, y + 20);
            }
        }

        // Horizontal road lane markers
        for (int i = 0; i < 3; i++) {
            int laneY = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + (i * LANE_WIDTH) + LANE_WIDTH/2;

            // Left section
            for (int x = 20; x < WINDOW_WIDTH/2 - ROAD_WIDTH/2 - 20; x += 40) {
                SDL_RenderDrawLine(renderer, x, laneY, x + 20, laneY);
            }

            // Right section
            for (int x = WINDOW_WIDTH/2 + ROAD_WIDTH/2 + 20; x < WINDOW_WIDTH - 20; x += 40) {
                SDL_RenderDrawLine(renderer, x, laneY, x + 20, laneY);
            }
        }
    }

    void drawLaneLabels() {
        // Lane positions (centered in each lane)
        string laneNames[4] = {"AL2", "BL2", "CL2", "DL2"};
        SDL_Color laneColors[4] = {
            {220, 50, 50, 255},    // AL2 - Red
            {50, 220, 50, 255},    // BL2 - Green
            {50, 50, 220, 255},    // CL2 - Blue
            {220, 220, 50, 255}    // DL2 - Yellow
        };

        // Calculate lane center positions
        int verticalCenter = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + LANE_WIDTH + LANE_WIDTH/2;  // Middle lane (lane 1)
        int horizontalCenter = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + LANE_WIDTH + LANE_WIDTH/2;

        // Draw lane name backgrounds with TTF text
        SDL_Color textColor = {255, 255, 255, 255}; // White text

        // AL2 - Top lane (middle lane of top road)
        SDL_SetRenderDrawColor(renderer, laneColors[0].r, laneColors[0].g, laneColors[0].b, 200);
        SDL_Rect al2Bg = {verticalCenter - 30, 80, 60, 30};
        SDL_RenderFillRect(renderer, &al2Bg);
        renderTextCentered("AL2", verticalCenter, 85, textColor);

        // BL2 - Right lane (middle lane of right road)
        SDL_SetRenderDrawColor(renderer, laneColors[1].r, laneColors[1].g, laneColors[1].b, 200);
        SDL_Rect bl2Bg = {WINDOW_WIDTH - 110, horizontalCenter - 15, 60, 30};
        SDL_RenderFillRect(renderer, &bl2Bg);
        renderTextCentered("BL2", WINDOW_WIDTH - 80, horizontalCenter - 10, textColor);

        // CL3 - Bottom lane (middle lane of bottom road)
        SDL_SetRenderDrawColor(renderer, laneColors[2].r, laneColors[2].g, laneColors[2].b, 200);
        SDL_Rect cl2Bg = {verticalCenter - 30, WINDOW_HEIGHT - 110, 60, 30};
        SDL_RenderFillRect(renderer, &cl2Bg);
        renderTextCentered("CL2", verticalCenter, WINDOW_HEIGHT - 105, textColor);

        // DL4 - Left lane (middle lane of left road)
        SDL_SetRenderDrawColor(renderer, laneColors[3].r, laneColors[3].g, laneColors[3].b, 200);
        SDL_Rect dl2Bg = {50, horizontalCenter - 15, 60, 30};
        SDL_RenderFillRect(renderer, &dl2Bg);
        renderTextCentered("DL2", 80, horizontalCenter - 10, textColor);

        // Draw black borders
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &al2Bg);
        SDL_RenderDrawRect(renderer, &bl2Bg);
        SDL_RenderDrawRect(renderer, &cl2Bg);
        SDL_RenderDrawRect(renderer, &dl2Bg);
    }

    void drawVehicleQueues() {
        // Calculate lane center positions (middle lane - lane 1)
        int verticalCenter = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + LANE_WIDTH + LANE_WIDTH/2;
        int horizontalCenter = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + LANE_WIDTH + LANE_WIDTH/2;

        // Lane colors
        SDL_Color laneColors[4] = {
            {255, 100, 100, 255},  // AL2 - Red
            {100, 255, 100, 255},  // BL2 - Green
            {100, 100, 255, 255},  // CL3 - Blue
            {255, 255, 100, 255}   // DL4 - Yellow
        };

        // Draw vehicles in each lane (centered)
        for (int laneIdx = 0; laneIdx < LANES; laneIdx++) {
            int vehicleCount = lanes[laneIdx].vehicles.size();
            int maxToShow = 6;

            SDL_SetRenderDrawColor(renderer, laneColors[laneIdx].r, laneColors[laneIdx].g,
                                  laneColors[laneIdx].b, 255);

            // Draw waiting vehicles centered in their lane
            for (int i = 0; i < min(vehicleCount, maxToShow); i++) {
                SDL_Rect vehicleRect;

                switch(laneIdx) {
                    case 0: // AL2 - Top lane (vertical, middle lane)
                        vehicleRect = {verticalCenter - 10, 150 + i * 25, 20, 15};
                        break;
                    case 1: // BL2 - Right lane (horizontal, middle lane)
                        vehicleRect = {WINDOW_WIDTH - 150 - i * 25, horizontalCenter - 7, 15, 20};
                        break;
                    case 2: // CL3 - Bottom lane (vertical, middle lane)
                        vehicleRect = {verticalCenter - 10, WINDOW_HEIGHT - 150 - i * 25, 20, 15};
                        break;
                    case 3: // DL4 - Left lane (horizontal, middle lane)
                        vehicleRect = {150 + i * 25, horizontalCenter - 7, 15, 20};
                        break;
                }

                SDL_RenderFillRect(renderer, &vehicleRect);

                // Black outline
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &vehicleRect);

                // Reset lane color
                SDL_SetRenderDrawColor(renderer, laneColors[laneIdx].r, laneColors[laneIdx].g,
                                      laneColors[laneIdx].b, 255);
            }

            // Show count if more vehicles than shown
            if (vehicleCount > maxToShow) {
                string countText = "+" + to_string(vehicleCount - maxToShow);
                SDL_Color textColor = {0, 0, 0, 255};

                switch(laneIdx) {
                    case 0: // AL2
                        renderTextCentered(countText, verticalCenter, 150 + maxToShow * 25 + 5, textColor);
                        break;
                    case 1: // BL2
                        renderTextCentered(countText, WINDOW_WIDTH - 150 - maxToShow * 25 - 7,
                                          horizontalCenter, textColor);
                        break;
                    case 2: // CL3
                        renderTextCentered(countText, verticalCenter, WINDOW_HEIGHT - 150 - maxToShow * 25 + 5,
                                          textColor);
                        break;
                    case 3: // DL4
                        renderTextCentered(countText, 150 + maxToShow * 25 + 7, horizontalCenter, textColor);
                        break;
                }
            }
        }
    }

    void drawTrafficLights() {
        // Light positions near each lane
        SDL_Point lightPos[4] = {
            {WINDOW_WIDTH/2 - ROAD_WIDTH/2 - 60, 200},           // AL2
            {WINDOW_WIDTH - 200, WINDOW_HEIGHT/2 - ROAD_WIDTH/2 - 60}, // BL2
            {WINDOW_WIDTH/2 + ROAD_WIDTH/2 + 20, WINDOW_HEIGHT - 200}, // CL3
            {200, WINDOW_HEIGHT/2 + ROAD_WIDTH/2 + 20}          // DL4
        };

        for (int i = 0; i < LANES; i++) {
            // Light box
            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
            SDL_Rect lightBox = {lightPos[i].x, lightPos[i].y, 40, 100};
            SDL_RenderFillRect(renderer, &lightBox);

            // Red light
            if (lights[i]) {
                SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            SDL_Rect redLight = {lightPos[i].x + 5, lightPos[i].y + 5, 30, 30};
            SDL_RenderFillRect(renderer, &redLight);

            // Green light
            if (lights[i]) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
            }
            SDL_Rect greenLight = {lightPos[i].x + 5, lightPos[i].y + 65, 30, 30};
            SDL_RenderFillRect(renderer, &greenLight);

            // Black borders
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &lightBox);
            SDL_RenderDrawRect(renderer, &redLight);
            SDL_RenderDrawRect(renderer, &greenLight);
        }
    }

    void drawCycleInfo() {
        // Draw cycle info box
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 220);
        SDL_Rect infoBox = {10, 10, 250, 80};
        SDL_RenderFillRect(renderer, &infoBox);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &infoBox);

        // Draw text with TTF
        SDL_Color textColor = {0, 0, 0, 255};
        renderText("Cycle: " + to_string(cycleCount), 20, 20, textColor);

        if (currentGreenLane >= 0) {
            renderText("Green Light: " + lanes[currentGreenLane].name, 20, 45, textColor);

            // Show priority for AL2
            if (currentGreenLane == 0) {
                int priority = lanePQ.getPriority(0);
                string priorityText = "AL2 Priority: ";
                if (priority == 100) priorityText += "HIGH";
                else if (priority == 50) priorityText += "MEDIUM";
                else priorityText += to_string(priority);
                renderText(priorityText, 20, 70, textColor);
            }
        }
    }

    void displayConsoleStatus() {
        cout << "\n═══════════════════════════════════════════════════" << endl;
        cout << "CYCLE " << cycleCount << endl;
        cout << "═══════════════════════════════════════════════════" << endl;

        cout << "Lane\tVehicles\tPriority\tLight" << endl;
        cout << "----\t--------\t--------\t-----" << endl;

        for (int i = 0; i < LANES; i++) {
            cout << lanes[i].name << "\t"
                 << lanes[i].vehicles.size() << "\t\t"
                 << lanePQ.getPriority(i) << "\t\t"
                 << (lights[i] ? "GREEN" : "RED") << endl;
        }
    }

    void runCycle() {
        cycleCount++;

        // Read new vehicles
        readVehicleData();

        // Update priorities
        updatePriorities();

        // Select and serve lane
        int laneToServe = selectLaneToServe();
        if (laneToServe >= 0) {
            setTrafficLights(laneToServe);
            serveVehicles(laneToServe);
        }

        // Display status
        displayConsoleStatus();

        // Update visualization
        render();

        // Re-insert served lane
        if (laneToServe >= 0) {
            lanePQ.updatePriority(laneToServe, lanes[laneToServe].vehicles.size());
        }
    }

    void runSimulation(int totalCycles) {
        cout << "\n TRAFFIC SIMULATION STARTING " << endl;
        cout << "Priority Lane: AL2" << endl;
        cout << "Running " << totalCycles << " cycles..." << endl;
        cout << "==============================================" << endl;

        // Initial render
        render();

        for (int i = 0; i < totalCycles; i++) {
            runCycle();

            // Wait 2 seconds between cycles
            if (i < totalCycles - 1) {
                this_thread::sleep_for(chrono::seconds(2));
            }

            // Check for window close
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    cout << "\nSimulation stopped by user." << endl;
                    return;
                }
            }
        }

        cout << "\n SIMULATION COMPLETED: " << totalCycles << " cycles" << endl;

        // Keep window open until user closes it
        bool running = true;
        while (running) {
            render();
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) running = false;
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
};

int main() {
    TrafficSimulator simulator;
    simulator.runSimulation(15);  // Run 15 cycles

    return 0;
}
