#include "game.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdio.h>

#include "chess.h"
#include "constants.h"
#include "path.h"

typedef struct {
    SDL_Texture *spritesheet;
    SDL_Texture *square;
    SDL_Color white;
    SDL_Color black;
} Resources;

void screen_to_board_coords(const Board* board, int x, int y, int *new_x, int *new_y);

void render_board(SDL_Renderer *renderer, const Resources *resources,
                  const Board *board);

void free_resources(Resources *);

void game(SDL_Renderer *renderer) {
    Resources resources = {
        .spritesheet = IMG_LoadTexture(renderer, get_path("../gfx/Pieces.png")),
        .square = IMG_LoadTexture(renderer, get_path("../gfx/Square.png")),
        .white = {255, 255, 255, 255},
        .black = {50, 50, 50, 255}};

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
                case SDL_MOUSEBUTTONDOWN:
                    printf("Click Detect!\n");
                    int x, y;

                    SDL_GetMouseState(&x, &y);

                    screen_to_board_coords(&board, x, y, &x, &y);

                    printf("%d, %d\n", x, y);

                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render_board(renderer, &resources, &board);

        SDL_RenderPresent(renderer);
    }
}

void screen_to_board_coords(const Board* board, int x, int y, int *new_x, int *new_y) {
     *new_x = x / (WINDOW_WIDTH / board->width);
     *new_y = y / (WINDOW_HEIGHT / board->height);
}

// This needs refactored asap
void render_board(SDL_Renderer *renderer, const Resources *resources,
                  const Board *board) {
    for (int x = 0; x < board->width; x++) {
        for (int y = 0; y < board->height; y++) {
            SDL_Color color =
                (x + y) % 2 == 0 ? resources->white : resources->black;

            SDL_SetTextureColorMod(resources->square, color.r, color.g,
                                   color.b);

            int cell_width = WINDOW_WIDTH / board->width;
            int cell_height = WINDOW_HEIGHT / board->height;

            SDL_Rect square_rect = {.x = x * cell_width,
                                    .y = y * cell_height,
                                    .w = cell_width,
                                    .h = cell_height};

            SDL_RenderCopy(renderer, resources->square, NULL, &square_rect);

            int index = x + y * board->width;

            if (board->grid[index].type == NONE) continue;

            int width;
            int height;

            SDL_QueryTexture(resources->spritesheet, NULL, NULL, &width,
                             &height);

            width /= 6;
            height /= 2;

            SDL_Rect s_rect = {width * board->grid[index].type,
                               height * board->grid[index].team, width, height};

            SDL_Rect rect = {x * cell_width, y * cell_height, cell_width,
                             cell_height};

            SDL_RenderCopy(renderer, resources->spritesheet, &s_rect, &rect);
        }
    }
}

void free_resources(Resources *resources) {
    SDL_DestroyTexture(resources->spritesheet);
    SDL_DestroyTexture(resources->square);
}
