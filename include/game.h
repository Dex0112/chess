#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL_render.h>
#include "chess.h"
#include "clock.h"

typedef enum {
    STATE_IDLE,
    STATE_PIECE_SELECTED,
    STATE_DRAGGING_PIECE
} GameState;

typedef struct {
    Clock *clock;
    Board *board;
    GameState game_state;
    SDL_Point selected;
} AppState;

typedef struct {
    // Should this be a board point with a board_size
    SDL_Rect board;
    // Refactor to have a clock x width and height
    // then a white y and a black y
    // To even further reduce code duplication
    // I belive SDL_Rect to be the incorrect abstraction
    // Belongs in clock precomputed
    unsigned int clock_width;
    unsigned int clock_height;
    int white_clock_y;
    int black_clock_y;
    int clock_x;
    SDL_Rect white_clock;
    SDL_Rect black_clock;
} ScreenLayout;

void game(SDL_Renderer *);

#endif
