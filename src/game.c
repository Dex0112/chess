#include "game.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdio.h>

#include "chess.h"
#include "constants.h"

typedef struct {
    SDL_Texture *spritesheet;
    SDL_Texture *square;
    SDL_Color white;
    SDL_Color black;
} Resources;

void render_board(SDL_Renderer *renderer, const Resources *resources,
                  const Board *board);

void game(SDL_Renderer *renderer) {
    printf("Entering Game\n");

    Resources resources = {
        .spritesheet = IMG_LoadTexture(renderer, "../gfx/Pieces.png"),
        .square = IMG_LoadTexture(renderer, "../gfx/Square.png"),
        .white = {255, 255, 255, 255},
        .black = {0, 0, 0, 255}};

    Board board = {
        .width = 8,
        .height = 8,
    };

    load_fen(&board, STARTING_FEN);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render_board(renderer, &resources, &board);

        SDL_RenderPresent(renderer);
    }
}

void render_board(SDL_Renderer *renderer, const Resources *resources,
                  const Board *board) {
    for (int x = 0; x < board->width; x++) {
        for (int y = 0; y < board->height; y++) {
            SDL_Color color =
                (x + y) % 2 == 0 ? resources->black : resources->white;

            SDL_SetTextureColorMod(resources->square, color.r, color.g,
                                   color.b);

            int cell_width = WINDOW_WIDTH / board->width;
            int cell_height = WINDOW_HEIGHT / board->height;

            SDL_Rect rect = {.x = x * cell_width,
                             .y = y * cell_height,
                             .w = cell_width,
                             .h = cell_height};

            SDL_RenderCopy(renderer, resources->square, NULL, &rect);

            // This might cause a segfault but I think I fixed it
            // int index = x + y * board->width;
            // if (board->grid[index].type == NONE) continue;
            // Render Piece
        }
    }
}
