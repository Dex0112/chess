#ifndef CHESS_RESOURCES
#define CHESS_RESOURCES

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

typedef struct {
    SDL_Texture *sprite_sheet;
    SDL_Color white_color;
    SDL_Color black_color;
} ChessResources;

extern const ChessResources *CHESS_RESOURCES_DEFAULTS;

void CHESS_RESOURCES_init();

#endif
