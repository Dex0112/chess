#ifndef CLOCK_CONTEXT

#define CLOCK_CONTEXT

#include <SDL2/SDL_ttf.h>

typedef struct {
    TTF_Font *clock_font;

    SDL_Texture *background;
    SDL_Color clock_active;
    SDL_Color clock_inactive;
    SDL_Color clock_text_active;
    SDL_Color clock_text_inactive;
} ClockResources;

ClockResources load_default_clock_resources(SDL_Renderer *renderer);

#endif
