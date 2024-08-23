#ifndef RENDER
#define RENDER

#include <SDL2/SDL_render.h>

#include "chess.h"

void render_init(SDL_Renderer *renderer);

void render_board(SDL_Renderer *renderer, const Board *board,
                  const SDL_Point *selected);

#endif
