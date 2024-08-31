#ifndef RESOURCES
#define RESOURCES

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include "chess.h"

// TODO: Implement using this
typedef struct {
    SDL_Texture *sprite_sheet;
    SDL_Texture *square;
    SDL_Color white;
    SDL_Color black;
    // I need a highlight light and highlight dark for light and dark squares
    SDL_Color light_square_highlight;
    SDL_Color dark_square_highlight;
} BoardResources;

// Should this be in a different file?
typedef struct {
TTF_Font *clock_font;
SDL_Color clock_active;
SDL_Color clock_inactive;
SDL_Color clock_text_active;
SDL_Color clock_text_inactive;
} ClockResources;

// Implement
SDL_Rect get_piece_src(SDL_Texture *sprite_sheet, Piece);

#endif
