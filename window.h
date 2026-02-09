#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"

class WindowManager {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    std::vector<std::vector<int>> screen;

    void draw() {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
        for(int i=0; i<32; ++i) {
            for(int j=0; j<64; ++j) {
                if(screen[i][j] != 0) {
                    const SDL_FRect rect {
                        .x = (float)j,
                        .y = (float)i,
                        .w = 1,
                        .h = 1
                    };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
    }
public:
    WindowManager() {
        screen = std::vector<std::vector<int>>(32, std::vector<int>(64, 0));
    }
    void addSquare(int x, int y) {
        if(0 > x && 
            x >= 64 &&
            0 > y &&
            y >= 32) {
            return;
        }
        screen[y][x] = 1;
    }
    bool initialize() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
            return false;
        }
        if (!SDL_CreateWindowAndRenderer("Hello SDL", 640, 320, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
            return false;
        }
        if (!SDL_SetRenderLogicalPresentation(renderer, 64, 32, SDL_LOGICAL_PRESENTATION_STRETCH)){

            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set logical presentation: %s", SDL_GetError());
            return false;
        }

        return true;
    }
    bool render() {
        SDL_PollEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }

        // background
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderClear(renderer);

        // draw squares
        draw();

        SDL_RenderPresent(renderer);

        return true;
    }

    void destroy() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

#endif
