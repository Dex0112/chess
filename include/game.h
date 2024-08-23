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

void game(SDL_Renderer *);

#endif
