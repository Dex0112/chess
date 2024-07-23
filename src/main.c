#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

typedef enum {
    KING = 0,
    QUEEN = 1,
    BISHOP = 2,
    KNIGHT = 3,
    ROOK = 4,
    PAWN = 5,
} PieceType;

void render_piece(SDL_Renderer *renderer, PieceType piece_type);

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("IMG_Init: %s\n", IMG_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Cnake", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 720,
                                          720, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_Event event;

    bool running = true;
    
    SDL_SetRenderDrawColor(renderer, 122, 122, 122, 255);

    render_piece(renderer, KING);
    render_piece(renderer, QUEEN);
    render_piece(renderer, BISHOP);
    render_piece(renderer, KNIGHT);
    render_piece(renderer, ROOK);
    render_piece(renderer, PAWN);

    SDL_RenderPresent(renderer);


    while(running) {
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

void render_piece(SDL_Renderer *renderer, PieceType piece_type) {
    // Find a proper chess spritesheet
    SDL_Texture *sprite_sheet = IMG_LoadTexture(renderer, "./gfx/pieces.png");

    int cell_width;
    int cell_height;

    SDL_QueryTexture(sprite_sheet, NULL, NULL, &cell_width, &cell_height);

    printf("W: %d\nH: %d\n", cell_width, cell_height);

    cell_width /= 6;
    cell_height /= 2;

    printf("Cell_W: %d\nCell_H: %d\n", cell_width, cell_height);


    SDL_Rect white_s_rect = {
        cell_width * piece_type,
        0,
        cell_width,
        cell_height
    };

    SDL_Rect black_s_rect = {
        cell_width * piece_type,
        cell_height,
        cell_width,
        cell_height,
    };


    SDL_Rect white_position = {
        100 * piece_type + 5 * piece_type,
        0,
        100,
        100,
    };

    SDL_Rect black_position = {
        100 * piece_type + 25,
        200,
        100,
        100,
    };

    SDL_RenderCopy(renderer, sprite_sheet, &white_s_rect, &white_position);
    SDL_RenderCopy(renderer, sprite_sheet, &black_s_rect, &black_position);

    SDL_DestroyTexture(sprite_sheet);
}
