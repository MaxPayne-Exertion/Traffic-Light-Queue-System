#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL not initialized SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    SDL_Window* window = SDL_CreateWindow("SDL2 Window",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "Window not gen SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer not gen SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    bool running = true;
    SDL_Event event;


    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }


        SDL_SetRenderDrawColor(renderer, 30, 30, 100, 255);  // Dark blue background
        SDL_RenderClear(renderer);

        // Draw a simple rectangle
        SDL_Rect rect = {WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2 - 50, 100, 100};
        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);  // Red rectangle
        SDL_RenderFillRect(renderer, &rect);


        SDL_RenderPresent(renderer);


        SDL_Delay(16);  // 60 FPS
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
