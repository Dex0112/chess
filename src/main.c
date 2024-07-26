#include "chess.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdio.h>

int main() {
    Board board = {
        8,
        8,
        NULL,
    };

    load_fen(&board, STARTING_FEN);

    return 0;

    SDL_Init(SDL_INIT_VIDEO);

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("IMG_Init: %s\n", IMG_GetError());
        return -1;
    }

    SDL_Window *window =
        SDL_CreateWindow("Cnake", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 720, 720, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_Event event;

    bool running = true;

    SDL_SetRenderDrawColor(renderer, 122, 122, 122, 255);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();
}
