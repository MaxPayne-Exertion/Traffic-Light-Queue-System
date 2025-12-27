#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include "queue.h"

const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 700;
const int LANE_WIDTH = 50;
const int VEHICLE_WIDTH = 45;
const int VEHICLE_HEIGHT = 35;
const int ROAD_LENGTH = 400;

enum LightState { RED = 0, GREEN = 1 };

struct AnimatedVehicle {
    std::string id;
    float x, y;
    float targetX, targetY;
    std::string lane;
    bool isMoving;
    SDL_Color color;
    int direction; // 0=right, 1=down, 2=left, 3=up

    AnimatedVehicle(std::string vid, float startX, float startY, std::string vlane, int dir)
        : id(vid), x(startX), y(startY), targetX(startX), targetY(startY),
          lane(vlane), isMoving(false), direction(dir) {
        color = {
            static_cast<Uint8>(50 + rand() % 156),
            static_cast<Uint8>(50 + rand() % 156),
            static_cast<Uint8>(50 + rand() % 156),
            255
        };
    }
};

class TrafficSimulatorSDL {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* smallFont;
    TTF_Font* tinyFont;

    Lane* laneA;
    Lane* laneB;
    Lane* laneC;
    Lane* laneD;

    LanePriorityQueue* lanePQ;
    std::map<std::string, LightState> trafficLights;

    std::vector<AnimatedVehicle> animatedVehicles;
    std::string currentGreenLane;

    int vehiclesProcessed;
    int timePerVehicle;
    int minVehiclesBeforePriority;

    Uint32 lastUpdateTime;
    Uint32 lastLoadTime;
    Uint32 lastProcessTime;

    bool running;

    // Center coordinates
    int centerX, centerY;
    int junctionSize;

public:
    TrafficSimulatorSDL() : window(nullptr), renderer(nullptr), font(nullptr),
                            smallFont(nullptr), tinyFont(nullptr), vehiclesProcessed(0),
                            timePerVehicle(2), minVehiclesBeforePriority(5),
                            lastUpdateTime(0), lastLoadTime(0), lastProcessTime(0), running(true) {

        centerX = WINDOW_WIDTH / 2;
        centerY = WINDOW_HEIGHT / 2;
        junctionSize = LANE_WIDTH * 6;

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return;
        }

        if (TTF_Init() < 0) {
            std::cerr << "TTF initialization failed: " << TTF_GetError() << std::endl;
            return;
        }

        window = SDL_CreateWindow("Traffic Light Queue System - SDL2",
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 WINDOW_WIDTH, WINDOW_HEIGHT,
                                 SDL_WINDOW_SHOWN);

        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        if (!renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            return;
        }

        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 20);
        smallFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
        tinyFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 12);

        if (!font) {
            font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 20);
            smallFont = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 16);
            tinyFont = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 12);
        }

        laneA = new Lane("AL2", true);
        laneB = new Lane("BL2", false);
        laneC = new Lane("CL2", false);
        laneD = new Lane("DL2", false);

        lanePQ = new LanePriorityQueue(4);
        lanePQ->insert(laneA);
        lanePQ->insert(laneB);
        lanePQ->insert(laneC);
        lanePQ->insert(laneD);

        trafficLights["AL2"] = RED;
        trafficLights["BL2"] = RED;
        trafficLights["CL2"] = RED;
        trafficLights["DL2"] = RED;

        currentGreenLane = "";
    }

    ~TrafficSimulatorSDL() {
        delete laneA;
        delete laneB;
        delete laneC;
        delete laneD;
        delete lanePQ;

        if (font) TTF_CloseFont(font);
        if (smallFont) TTF_CloseFont(smallFont);
        if (tinyFont) TTF_CloseFont(tinyFont);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);

        TTF_Quit();
        SDL_Quit();
    }

    void drawText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* textFont = nullptr) {
        if (!textFont) textFont = font;
        if (!textFont) return;

        SDL_Surface* surface = TTF_RenderText_Blended(textFont, text.c_str(), color);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_FreeSurface(surface);
            return;
        }

        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    void drawFilledCircle(int cx, int cy, int radius, SDL_Color color) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx*dx + dy*dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
                }
            }
        }
    }

    void drawRectWithBorder(SDL_Rect rect, SDL_Color fillColor, SDL_Color borderColor, int borderWidth = 2) {
        SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        for (int i = 0; i < borderWidth; i++) {
            SDL_Rect borderRect = {rect.x - i, rect.y - i, rect.w + 2*i, rect.h + 2*i};
            SDL_RenderDrawRect(renderer, &borderRect);
        }
    }

    void drawRoad() {
        SDL_Color roadColor = {70, 70, 70, 255};
        SDL_Color laneLineColor = {255, 255, 100, 255};

        int halfJunction = junctionSize / 2;

        // Horizontal roads (left and right of junction)
        SDL_Rect leftRoad = {0, centerY - halfJunction, centerX - halfJunction, junctionSize};
        SDL_Rect rightRoad = {centerX + halfJunction, centerY - halfJunction, WINDOW_WIDTH - (centerX + halfJunction), junctionSize};

        SDL_SetRenderDrawColor(renderer, roadColor.r, roadColor.g, roadColor.b, roadColor.a);
        SDL_RenderFillRect(renderer, &leftRoad);
        SDL_RenderFillRect(renderer, &rightRoad);

        // Vertical roads (top and bottom of junction)
        SDL_Rect topRoad = {centerX - halfJunction, 0, junctionSize, centerY - halfJunction};
        SDL_Rect bottomRoad = {centerX - halfJunction, centerY + halfJunction, junctionSize, WINDOW_HEIGHT - (centerY + halfJunction)};

        SDL_RenderFillRect(renderer, &topRoad);
        SDL_RenderFillRect(renderer, &bottomRoad);

        // Junction center
        SDL_Color junctionColor = {80, 80, 80, 255};
        SDL_Rect junction = {centerX - halfJunction, centerY - halfJunction, junctionSize, junctionSize};
        SDL_SetRenderDrawColor(renderer, junctionColor.r, junctionColor.g, junctionColor.b, junctionColor.a);
        SDL_RenderFillRect(renderer, &junction);

        // Draw lane dividers
        SDL_SetRenderDrawColor(renderer, laneLineColor.r, laneLineColor.g, laneLineColor.b, laneLineColor.a);

        // Horizontal lane lines (left road)
        for (int i = 1; i < 3; i++) {
            int y = centerY - halfJunction + i * LANE_WIDTH;
            for (int x = 0; x < centerX - halfJunction; x += 30) {
                SDL_Rect dash = {x, y - 1, 15, 2};
                SDL_RenderFillRect(renderer, &dash);
            }
        }

        // Horizontal lane lines (right road)
        for (int i = 1; i < 3; i++) {
            int y = centerY - halfJunction + i * LANE_WIDTH;
            for (int x = centerX + halfJunction; x < WINDOW_WIDTH; x += 30) {
                SDL_Rect dash = {x, y - 1, 15, 2};
                SDL_RenderFillRect(renderer, &dash);
            }
        }

        // Vertical lane lines (top road)
        for (int i = 1; i < 3; i++) {
            int x = centerX - halfJunction + i * LANE_WIDTH;
            for (int y = 0; y < centerY - halfJunction; y += 30) {
                SDL_Rect dash = {x - 1, y, 2, 15};
                SDL_RenderFillRect(renderer, &dash);
            }
        }

        // Vertical lane lines (bottom road)
        for (int i = 1; i < 3; i++) {
            int x = centerX - halfJunction + i * LANE_WIDTH;
            for (int y = centerY + halfJunction; y < WINDOW_HEIGHT; y += 30) {
                SDL_Rect dash = {x - 1, y, 2, 15};
                SDL_RenderFillRect(renderer, &dash);
            }
        }
    }

    void drawTrafficLight(int x, int y, LightState state, const std::string& label) {
        // Traffic light box background
        SDL_Color boxColor = {50, 50, 50, 255};
        SDL_Rect box = {x, y, 30, 70};
        SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        SDL_RenderFillRect(renderer, &box);

        // Border
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderDrawRect(renderer, &box);

        // Red light (top)
        SDL_Color redColor = (state == RED) ? SDL_Color{255, 50, 50, 255} : SDL_Color{80, 20, 20, 255};
        drawFilledCircle(x + 15, y + 18, 10, redColor);

        // Green light (bottom)
        SDL_Color greenColor = (state == GREEN) ? SDL_Color{50, 255, 50, 255} : SDL_Color{20, 80, 20, 255};
        drawFilledCircle(x + 15, y + 52, 10, greenColor);

        // Label
        if (smallFont) {
            SDL_Color textColor = {255, 255, 255, 255};
            drawText(label, x - 8, y + 75, textColor, smallFont);
        }
    }

    void drawVehicle(const AnimatedVehicle& vehicle) {
        int w = VEHICLE_WIDTH;
        int h = VEHICLE_HEIGHT;

        // Adjust size based on direction (vertical vs horizontal)
        if (vehicle.direction == 1 || vehicle.direction == 3) { // Up or down
            std::swap(w, h);
        }

        SDL_Rect body = {(int)(vehicle.x - w/2), (int)(vehicle.y - h/2), w, h};

        // Vehicle body
        drawRectWithBorder(body, vehicle.color, {0, 0, 0, 255}, 2);

        // Vehicle ID
        if (tinyFont) {
            SDL_Color textColor = {255, 255, 255, 255};
            std::string shortId = vehicle.id.substr(0, 4);
            int textX = (int)vehicle.x - 12;
            int textY = (int)vehicle.y - 6;
            drawText(shortId, textX, textY, textColor, tinyFont);
        }
    }

    void drawLaneInfo() {
        int panelX = 20;
        int panelY = 20;
        int panelWidth = 250;
        int panelHeight = 200;

        // Background
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Color panelBg = {0, 0, 0, 200};
        SDL_Rect panel = {panelX, panelY, panelWidth, panelHeight};
        SDL_SetRenderDrawColor(renderer, panelBg.r, panelBg.g, panelBg.b, panelBg.a);
        SDL_RenderFillRect(renderer, &panel);

        SDL_Color borderColor = {100, 100, 255, 255};
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawRect(renderer, &panel);

        if (!smallFont) return;

        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Color priorityColor = {255, 100, 100, 255};
        SDL_Color greenColor = {100, 255, 100, 255};

        int yPos = panelY + 10;

        drawText("TRAFFIC STATUS", panelX + 10, yPos, {255, 255, 100, 255}, smallFont);
        yPos += 25;

        // Lane A (Priority)
        std::string aText = "AL2 (L): " + std::to_string(laneA->getVehicleCount());
        if (laneA->priority > 0) aText += " [P]";
        drawText(aText, panelX + 10, yPos, laneA->priority > 0 ? priorityColor : textColor, smallFont);
        yPos += 22;

        // Lane B
        std::string bText = "BL2 (T): " + std::to_string(laneB->getVehicleCount());
        drawText(bText, panelX + 10, yPos, textColor, smallFont);
        yPos += 22;

        // Lane C
        std::string cText = "CL2 (R): " + std::to_string(laneC->getVehicleCount());
        drawText(cText, panelX + 10, yPos, textColor, smallFont);
        yPos += 22;

        // Lane D
        std::string dText = "DL2 (B): " + std::to_string(laneD->getVehicleCount());
        drawText(dText, panelX + 10, yPos, textColor, smallFont);
        yPos += 30;

        // Current green
        std::string greenText = "GREEN: " + (currentGreenLane.empty() ? "NONE" : currentGreenLane);
        drawText(greenText, panelX + 10, yPos, greenColor, smallFont);
        yPos += 22;

        // Total processed
        std::string procText = "Passed: " + std::to_string(vehiclesProcessed);
        drawText(procText, panelX + 10, yPos, textColor, smallFont);
    }

    void drawAnimatedVehicles() {
        for (const auto& vehicle : animatedVehicles) {
            drawVehicle(vehicle);
        }
    }

    void loadVehiclesFromFiles() {
        std::string laneFiles[] = {"lanea.txt", "laneb.txt", "lanec.txt", "laned.txt"};
        Lane* lanes[] = {laneA, laneB, laneC, laneD};

        for (int i = 0; i < 4; i++) {
            std::ifstream inFile(laneFiles[i]);
            if (inFile.is_open()) {
                std::string line;
                while (std::getline(inFile, line)) {
                    std::stringstream ss(line);
                    std::string id, timeStr, laneName;

                    std::getline(ss, id, ',');
                    std::getline(ss, timeStr, ',');
                    std::getline(ss, laneName, ',');

                    if (!id.empty()) {
                        time_t arrivalTime = std::stoi(timeStr);
                        Vehicle v(id, arrivalTime, laneName);
                        lanes[i]->vehicleQueue->enqueue(v);
                    }
                }
                inFile.close();

                std::ofstream clearFile(laneFiles[i], std::ios::trunc);
                clearFile.close();
            }
        }
    }

    void updateAnimations(float deltaTime) {
        for (auto& vehicle : animatedVehicles) {
            if (vehicle.isMoving) {
                float dx = vehicle.targetX - vehicle.x;
                float dy = vehicle.targetY - vehicle.y;
                float distance = sqrt(dx * dx + dy * dy);

                if (distance < 5.0f) {
                    vehicle.x = vehicle.targetX;
                    vehicle.y = vehicle.targetY;
                    vehicle.isMoving = false;
                } else {
                    float speed = 250.0f * deltaTime;
                    vehicle.x += (dx / distance) * speed;
                    vehicle.y += (dy / distance) * speed;
                }
            }
        }

        // Remove vehicles that are off-screen
        animatedVehicles.erase(
            std::remove_if(animatedVehicles.begin(), animatedVehicles.end(),
                [](const AnimatedVehicle& v) {
                    return !v.isMoving &&
                           (v.x < -100 || v.x > WINDOW_WIDTH + 100 ||
                            v.y < -100 || v.y > WINDOW_HEIGHT + 100);
                }),
            animatedVehicles.end()
        );
    }

    void processLane(Lane* lane) {
        if (lane->getVehicleCount() == 0) return;

        int vehiclesToProcess;
        if (lane->priority > 0) {
            vehiclesToProcess = lane->getVehicleCount() - minVehiclesBeforePriority;
            if (vehiclesToProcess < 0) vehiclesToProcess = 0;
        } else {
            int total = 0, count = 0;
            if (laneA->priority == 0) { total += laneA->getVehicleCount(); count++; }
            if (laneB->priority == 0) { total += laneB->getVehicleCount(); count++; }
            if (laneC->priority == 0) {dividers total += laneC->getVehicleCount(); count++; }
            if (laneD->priority == 0) { total += laneD->getVehicleCount(); count++; }
            vehiclesToProcess = count > 0 ? (total / count) : 1;
            if (vehiclesToProcess > lane->getVehicleCount()) {
                vehiclesToProcess = lane->getVehicleCount();
            }
            if (vehiclesToProcess == 0 && lane->getVehicleCount() > 0) {
                vehiclesToProcess = 1;
            }
        }

        setTrafficLight(lane->name, GREEN);

        int halfJunction = junctionSize / 2;

        for (int i = 0; i < vehiclesToProcess && !lane->vehicleQueue->isEmpty(); i++) {
            Vehicle v = lane->vehicleQueue->dequeue();
            vehiclesProcessed++;

            float startX, startY, endX, endY;
            int direction;

            if (lane->name == "AL2") {
                // Left lane - vehicles enter from left, exit right
                startX = 50;
                startY = centerY - halfJunction + LANE_WIDTH * 1.5;
                endX = WINDOW_WIDTH + 50;
                endY = startY;
                direction = 0; // right
            } else if (lane->name == "BL2") {
                // Top lane - vehicles enter from top, exit bottom
                startX = centerX - halfJunction + LANE_WIDTH * 1.5;
                startY = 50;
                endX = startX;
                endY = WINDOW_HEIGHT + 50;
                direction = 1; // down
            } else if (lane->name == "CL2") {
                // Right lane - vehicles enter from right, exit left
                startX = WINDOW_WIDTH - 50;
                startY = centerY + halfJunction - LANE_WIDTH * 1.5;
                endX = -50;
                endY = startY;
                direction = 2; // left
            } else { // DL2
                // Bottom lane - vehicles enter from bottom, exit top
                startX = centerX + halfJunction - LANE_WIDTH * 1.5;
                startY = WINDOW_HEIGHT - 50;
                endX = startX;
                endY = -50;
                direction = 3; // up
            }

            AnimatedVehicle av(v.id, startX, startY, lane->name, direction);
            av.targetX = endX;
            av.targetY = endY;
            av.isMoving = true;
            animatedVehicles.push_back(av);
        }
    }

    void setTrafficLight(std::string laneName, LightState state) {
        if (state == GREEN) {
            for (auto& light : trafficLights) {
                light.second = RED;
            }
            trafficLights[laneName] = GREEN;
            currentGreenLane = laneName;
        } else {
            trafficLights[laneName] = RED;
            if (currentGreenLane == laneName) {
                currentGreenLane = "";
            }
        }
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }
    }

    void run() {
        if (!window || !renderer) {
            std::cerr << "Failed to initialize SDL" << std::endl;
            return;
        }

        std::cout << "SDL2 Traffic Simulator Started" << std::endl;
        std::cout << "Press ESC to exit" << std::endl;

        Uint32 lastFrameTime = SDL_GetTicks();

        while (running) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
            lastFrameTime = currentTime;

            handleEvents();

            // Load vehicles every 2 seconds
            if (currentTime - lastLoadTime > 2000) {
                loadVehiclesFromFiles();
                lastLoadTime = currentTime;
            }

            // Update priorities every 1 second
            if (currentTime - lastUpdateTime > 1000) {
                laneA->updatePriority();
                laneB->updatePriority();
                laneC->updatePriority();
                laneD->updatePriority();
                lanePQ->updatePriorities();
                lastUpdateTime = currentTime;
            }

            // Process lanes every 3 seconds
            if (currentTime - lastProcessTime > 3000) {
                if (!lanePQ->isEmpty()) {
                    Lane* currentLane = lanePQ->extractMax();
                    if (currentLane->getVehicleCount() > 0) {
                        processLane(currentLane);
                    }
                    lanePQ->insert(currentLane);
                }
                lastProcessTime = currentTime;
            }

            // Update animations
            updateAnimations(deltaTime);

            // Render
            SDL_SetRenderDrawColor(renderer, 40, 120, 40, 255); // Dark green grass
            SDL_RenderClear(renderer);

            drawRoad();

            int halfJunction = junctionSize / 2;

            // Draw traffic lights beside their lanes
            // AL2 - Left side
            drawTrafficLight(centerX - halfJunction - 60, centerY - 35, trafficLights["AL2"], "AL2");

            // BL2 - Top
            drawTrafficLight(centerX - 15, centerY - halfJunction - 100, trafficLights["BL2"], "BL2");

            // CL2 - Right side
            drawTrafficLight(centerX + halfJunction + 30, centerY - 35, trafficLights["CL2"], "CL2");

            // DL2 - Bottom
            drawTrafficLight(centerX - 15, centerY + halfJunction + 30, trafficLights["DL2"], "DL2");

            drawAnimatedVehicles();
            drawLaneInfo();

            SDL_RenderPresent(renderer);

            SDL_Delay(16); // ~60 FPS
        }
    }
};

int main(int argc, char* argv[]) {
    srand(time(nullptr));

    TrafficSimulatorSDL simulator;
    simulator.run();

    return 0;
}
