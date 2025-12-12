#include <SDL2/SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

int main() {
    // initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // create window
    SDL_Window* window = SDL_CreateWindow("4-Way Intersection",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Road dimensions
    const int roadwidth = 300;              // total width  (3 lanes + margins)
    const int lanewidth = 80;               // width of each lane
    const int lanemargin = 10;              // margin between lanes


    bool running = true;
    SDL_Event event;

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        
        SDL_SetRenderDrawColor(renderer,255, 255, 255, 1);  //white
        SDL_RenderClear(renderer);

        // draw vertical road (UP DOWN)
        int verticalX = SCREEN_WIDTH / 2 - roadwidth / 2;
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);  // Road gray
        SDL_Rect verticalRoad = {verticalX, 0, roadwidth, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &verticalRoad);

        // draw horizontal road (LEFT RIGHT)
        int horizontalY = SCREEN_HEIGHT / 2 - roadwidth / 2;
        SDL_Rect horizontalRoad = {0, horizontalY, SCREEN_WIDTH, roadwidth};
        SDL_RenderFillRect(renderer, &horizontalRoad);


        // draw lane dividers
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White

        // Calculate positions for vertical road lanes (UP DOWN)
        for (int i = 1; i <= 2; i++) {  // 4 dividers for 3 lanes
            int laneX = verticalX + i * lanewidth + i * lanemargin;

            // Top part
            SDL_RenderDrawLine(renderer, laneX, 20, laneX, horizontalY - 20);


            // Bottom part

            SDL_RenderDrawLine(renderer, laneX, horizontalY+roadwidth+2, laneX, SCREEN_HEIGHT -20);

        }

        // Calculate positions for horizontal road lanes (LEFT RIGHT)
        for (int i = 1; i <= 2; i++) {  // 4 dividers for 3 lanes
            int laneY = horizontalY + i * lanewidth + i * lanemargin;

            // Left part 

            SDL_RenderDrawLine(renderer, 20, laneY, verticalX - 20, laneY);


            // Right part

            SDL_RenderDrawLine(renderer, verticalX+roadwidth+20, laneY, SCREEN_WIDTH-20, laneY);

        }

        // Update the screen
        SDL_RenderPresent(renderer);

        // Small delay
        SDL_Delay(16);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
