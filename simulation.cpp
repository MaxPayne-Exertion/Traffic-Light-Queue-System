#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>

// Include your existing header
#include "queue.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int CAR_SIZE = 24;
const float MAX_SPEED = 4.0f;

// Logic Thresholds
const int PRIORITY_START = 10;
const int PRIORITY_END = 5;


SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
TTF_Font* fontLarge = nullptr;

// Data Structures
Lane* pqLanes[4]; // 0=A(AL2), 1=B(BL2), 2=C(CL2), 3=D(DL2)
Queue<Vehicle>* myQueues[4];
LanePriorityQueue* pq = nullptr;

// Simulation State
bool priorityMode = false;
int currentCycleIndex = 0;
Uint32 lastCycleTime = 0;
int totalVehiclesPassed = 0; // New Statistic

// Visualization Data
struct VisualCar {
    std::string id;
    float x, y;
    float speed;
    int laneIndex;
    int state;
    std::string laneLabel;
};

std::vector<VisualCar> trafficVisuals;


bool carExists(std::string id) {
    for(const auto& c : trafficVisuals) if(c.id == id) return true;
    return false;
}

// Counts visually waiting cars for logic triggers
int getVisualQueueCount(int laneIdx) {
    int count = 0;
    for(const auto& c : trafficVisuals) {
        if(c.laneIndex == laneIdx && c.state == 1) count++;
    }
    return count;
}

void initSDL() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("Traffic Control System", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load Fonts
    font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 18);
    if(!font) font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.tt", 18);

    fontLarge = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.tt", 24);
    if(!fontLarge) fontLarge = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.tt", 24);
}



void loadTraffic() {
    std::string files[] = {"lanea.txt", "laneb.txt", "lanec.txt", "laned.txt"};
    std::string labels[] = {"AL2", "BL2", "CL2", "DL2"};

    for(int i=0; i<4; i++) {
        std::ifstream f(files[i]);
        if(!f.is_open()) continue;

        std::string line;
        while(std::getline(f, line)) {
            if(line.empty()) continue;
            std::stringstream ss(line);
            std::string id, tStr, lName;
            std::getline(ss, id, ','); std::getline(ss, tStr, ','); std::getline(ss, lName, ',');

            if(!carExists(id)) {
                Vehicle v(id, (time_t)std::stoi(tStr), lName);
                myQueues[i]->enqueue(v);

                VisualCar vc;
                vc.id = id; vc.laneIndex = i; vc.laneLabel = labels[i];
                vc.state = 0; vc.speed = MAX_SPEED;

                if(i == 0) { vc.x = 360; vc.y = -50; }       // A (North)
                else if(i == 1) { vc.x = 850; vc.y = 360; }  // B (East)
                else if(i == 2) { vc.x = 420; vc.y = 850; }  // C (South)
                else if(i == 3) { vc.x = -50; vc.y = 420; }  // D (West)

                trafficVisuals.push_back(vc);
            }
        }
        f.close();
        std::ofstream clear(files[i], std::ofstream::trunc);
    }
}

void updateLogic() {
    int countA = getVisualQueueCount(0); // Check AL2

    if(!priorityMode && countA >= PRIORITY_START) {
        priorityMode = true;
    } else if(priorityMode && countA < PRIORITY_END) {
        priorityMode = false;
        lastCycleTime = SDL_GetTicks();
    }

    for(int i=0; i<4; i++) pqLanes[i]->priority = 0;

    if(priorityMode) {
        pqLanes[0]->priority = 100;
    } else {
        if(SDL_GetTicks() - lastCycleTime > 2000) {
            currentCycleIndex = (currentCycleIndex + 1) % 4;
            lastCycleTime = SDL_GetTicks();
        }
        pqLanes[currentCycleIndex]->priority = 50;
    }

    while(!pq->isEmpty()) pq->extractMax();
    for(int i=0; i<4; i++) pq->insert(pqLanes[i]);
}

void updateVisuals() {
    Lane* activeLane = pq->extractMax();
    pq->insert(activeLane);

    int activeIndex = -1;
    for(int i=0; i<4; i++) {
        if(pqLanes[i] == activeLane) { activeIndex = i; break; }
    }

    int qCounts[4] = {0, 0, 0, 0};

    for(auto& c : trafficVisuals) {
        bool isGreen = (c.laneIndex == activeIndex);

        if(c.state != 2) {
            float target = 0;
            int qIdx = qCounts[c.laneIndex]++;
            float spacing = 32.0f;

            if(c.laneIndex == 0) target = 280 - (qIdx * spacing);
            else if(c.laneIndex == 1) target = 520 + (qIdx * spacing);
            else if(c.laneIndex == 2) target = 520 + (qIdx * spacing);
            else if(c.laneIndex == 3) target = 280 - (qIdx * spacing);

            float moveStep = c.speed;
            bool reached = false;

            if(c.laneIndex == 0) { if(c.y < target-5) c.y += moveStep; else { c.y=target; reached=true; }}
            else if(c.laneIndex == 1) { if(c.x > target+5) c.x -= moveStep; else { c.x=target; reached=true; }}
            else if(c.laneIndex == 2) { if(c.y > target+5) c.y -= moveStep; else { c.y=target; reached=true; }}
            else if(c.laneIndex == 3) { if(c.x < target-5) c.x += moveStep; else { c.x=target; reached=true; }}

            c.state = reached ? 1 : 0;

            if(isGreen && qIdx == 0 && reached) {
                static Uint32 lastDispatch = 0;
                if(SDL_GetTicks() - lastDispatch > 500) {
                    c.state = 2;
                    lastDispatch = SDL_GetTicks();
                    if(!myQueues[c.laneIndex]->isEmpty()) myQueues[c.laneIndex]->dequeue();

                    // INCREMENT STATS
                    totalVehiclesPassed++;
                }
            }
        }
        else {
            // Exiting Movement
            if(c.laneIndex == 0) c.y += MAX_SPEED * 1.5;
            else if(c.laneIndex == 1) c.x -= MAX_SPEED * 1.5;
            else if(c.laneIndex == 2) c.y -= MAX_SPEED * 1.5;
            else if(c.laneIndex == 3) c.x += MAX_SPEED * 1.5;
        }
    }

    auto it = std::remove_if(trafficVisuals.begin(), trafficVisuals.end(), [](const VisualCar& c){
        return c.x < -100 || c.x > 900 || c.y < -100 || c.y > 900;
    });
    trafficVisuals.erase(it, trafficVisuals.end());
}


void drawRect(int x, int y, int w, int h, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void drawText(int x, int y, std::string text, TTF_Font* f, SDL_Color color) {
    if(!f) return;
    SDL_Surface* surf = TTF_RenderText_Blended(f, text.c_str(), color);
    if(surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect r = {x, y, surf->w, surf->h};
        SDL_RenderCopy(renderer, tex, NULL, &r);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }
}

void render() {
    // 1. Background (Grass)
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
    SDL_RenderClear(renderer);

    // 2. Roads (Asphalt)
    drawRect(300, 0, 200, 800, 50, 50, 50); // Vertical
    drawRect(0, 300, 800, 200, 50, 50, 50); // Horizontal

    // 3. Road Markings (White Dashed Lines)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Vertical Center Lines
    for(int i=0; i<800; i+=40) {
        if(i > 280 && i < 520) continue; // Skip intersection
        SDL_Rect dash = {398, i, 4, 20};
        SDL_RenderFillRect(renderer, &dash);
    }
    // Horizontal Center Lines
    for(int i=0; i<800; i+=40) {
        if(i > 280 && i < 520) continue;
        SDL_Rect dash = {i, 398, 20, 4};
        SDL_RenderFillRect(renderer, &dash);
    }

    // Lane Dividers (Thinner)
    // Left of Center Vertical
    for(int i=0; i<800; i+=40) { if(i>280 && i<520) continue; SDL_Rect d = {333, i, 2, 20}; SDL_RenderFillRect(renderer, &d); }
    // Right of Center Vertical
    for(int i=0; i<800; i+=40) { if(i>280 && i<520) continue; SDL_Rect d = {466, i, 2, 20}; SDL_RenderFillRect(renderer, &d); }
    // Top of Center Horizontal
    for(int i=0; i<800; i+=40) { if(i>280 && i<520) continue; SDL_Rect d = {i, 333, 20, 2}; SDL_RenderFillRect(renderer, &d); }
    // Bottom of Center Horizontal
    for(int i=0; i<800; i+=40) { if(i>280 && i<520) continue; SDL_Rect d = {i, 466, 20, 2}; SDL_RenderFillRect(renderer, &d); }

    // 4. Zebra Crossings (Stripes at Stop Lines)
    // Vertical Road Crossings
    for(int x=305; x<500; x+=20) {
        drawRect(x, 280, 10, 20, 255, 255, 255); // Top
        drawRect(x, 500, 10, 20, 255, 255, 255); // Bottom
    }
    // Horizontal Road Crossings
    for(int y=305; y<500; y+=20) {
        drawRect(280, y, 20, 10, 255, 255, 255); // Left
        drawRect(500, y, 20, 10, 255, 255, 255); // Right
    }

    // 5. Intersection Box (Darker)
    drawRect(300, 300, 200, 200, 40, 40, 40);

    // 6. Get Active Lane for Lights
    Lane* active = pq->extractMax();
    pq->insert(active);

    // 7. Traffic Lights (with housings)
    struct LPos { int x, y; std::string n; };
    LPos lights[] = {{240, 240, "AL2"}, {540, 240, "BL2"}, {540, 540, "CL2"}, {240, 540, "DL2"}};

    for(int i=0; i<4; i++) {
        // Housing
        drawRect(lights[i].x, lights[i].y, 30, 30, 20, 20, 20);

        // Light Bulb
        bool isGreen = (pqLanes[i] == active);
        if(isGreen) drawRect(lights[i].x+5, lights[i].y+5, 20, 20, 0, 255, 0); // Green
        else drawRect(lights[i].x+5, lights[i].y+5, 20, 20, 255, 0, 0);       // Red

        // Lane Label
        drawText(lights[i].x, lights[i].y - 20, lights[i].n, font, {255, 255, 255});
    }

    // 8. Vehicles with Headlights
    for(const auto& c : trafficVisuals) {
        // Body
        SDL_Rect carBox = {(int)c.x, (int)c.y, CAR_SIZE, CAR_SIZE};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Border
        SDL_RenderDrawRect(renderer, &carBox);

        // Color
        if(c.laneIndex == 0) drawRect(c.x+1, c.y+1, CAR_SIZE-2, CAR_SIZE-2, 255, 215, 0); // Gold
        else drawRect(c.x+1, c.y+1, CAR_SIZE-2, CAR_SIZE-2, 65, 105, 225); // Royal Blue

        // Headlights (Yellow dots indicating direction)
        SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
        if(c.laneIndex == 0) { // Facing Down
            SDL_RenderDrawPoint(renderer, c.x+4, c.y+CAR_SIZE-2);
            SDL_RenderDrawPoint(renderer, c.x+CAR_SIZE-4, c.y+CAR_SIZE-2);
        } else if(c.laneIndex == 1) { // Facing Left
            SDL_RenderDrawPoint(renderer, c.x+2, c.y+4);
            SDL_RenderDrawPoint(renderer, c.x+2, c.y+CAR_SIZE-4);
        } else if(c.laneIndex == 2) { // Facing Up
            SDL_RenderDrawPoint(renderer, c.x+4, c.y+2);
            SDL_RenderDrawPoint(renderer, c.x+CAR_SIZE-4, c.y+2);
        } else if(c.laneIndex == 3) { // Facing Right
            SDL_RenderDrawPoint(renderer, c.x+CAR_SIZE-2, c.y+4);
            SDL_RenderDrawPoint(renderer, c.x+CAR_SIZE-2, c.y+CAR_SIZE-4);
        }
    }


    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect hud = {10, 10, 280, 120};
    SDL_RenderFillRect(renderer, &hud);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Text Lines
    drawText(20, 20, "TRAFFIC CONTROL", fontLarge, {255, 255, 255});

    std::string modeStr = priorityMode ? "Mode: PRIORITY (AL2)" : "Mode: NORMAL";
    SDL_Color modeCol = priorityMode ? SDL_Color{255, 100, 100} : SDL_Color{100, 255, 100};
    drawText(20, 50, modeStr, font, modeCol);

    std::string greenStr = "Green Lane: " + active->name;
    drawText(20, 75, greenStr, font, {255, 255, 255});

    std::string totalStr = "Passed Vehicles: " + std::to_string(totalVehiclesPassed);
    drawText(20, 100, totalStr, font, {200, 200, 255});

    SDL_RenderPresent(renderer);
}


int main(int argc, char* args[]) {
    initSDL();

    // Init Logic
    pqLanes[0] = new Lane("AL2", true);
    pqLanes[1] = new Lane("BL2", false);
    pqLanes[2] = new Lane("CL2", false);
    pqLanes[3] = new Lane("DL2", false);

    for(int i=0; i<4; i++) myQueues[i] = new Queue<Vehicle>();

    pq = new LanePriorityQueue(10);
    for(int i=0; i<4; i++) pq->insert(pqLanes[i]);

    bool running = true;
    SDL_Event e;

    while(running) {
        while(SDL_PollEvent(&e)) if(e.type == SDL_QUIT) running = false;

        loadTraffic();
        updateLogic();
        updateVisuals();
        render();

        SDL_Delay(16);
    }
    return 0;
}
