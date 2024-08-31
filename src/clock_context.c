#include "clock_context.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "path.h"

ClockResources load_default_clock_resources(SDL_Renderer *renderer) {
    ClockResources resources = {
        .clock_active = {.r = 255, .g = 255, .b = 255, .a = 255},
        .clock_inactive = {.r = 115, .g = 115, .b = 115, .a = 255},
        .background =
            IMG_LoadTexture(renderer, get_path("../gfx/ClockBackground.png")),
        .clock_font =
            TTF_OpenFont("/usr/share/fonts/gnu-free/FreeSansBold.otf", 25),
        .clock_text_active = {.r = 0, .g = 0, .b = 0, .a = 255},
        .clock_text_inactive = {.r = 0, .g = 0, .b = 0, .a = 255},
    };

    return resources;
}
