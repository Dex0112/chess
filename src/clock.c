#include "clock.h"

#include <SDL2/SDL_timer.h>
#include <stdlib.h>

void start_clock(Clock *chess_clock, int timer_seconds) {
    chess_clock->last_tick = SDL_GetTicks();

    chess_clock->current = (double *)malloc(sizeof(double));
    chess_clock->inactive = (double *)malloc(sizeof(double));

    *chess_clock->current = timer_seconds;
    *chess_clock->inactive = timer_seconds;
}

void clock_update(Clock *chess_clock) {
    const unsigned int MILLISECONDS_TO_SECONDS = 1000;

    unsigned int time = SDL_GetTicks();

    double delta = (double)(time - chess_clock->last_tick);

    delta /= MILLISECONDS_TO_SECONDS;

    *chess_clock->current -= delta;

    if (*chess_clock->current < 0) *chess_clock->current = 0;

    chess_clock->last_tick = time;
}

// You should call clock_update before calling clock_toggle
void clock_toggle(Clock *chess_clock) {
    // What the current and inactive pointers
    double *temp = chess_clock->current;

    chess_clock->current = chess_clock->inactive;

    chess_clock->inactive = temp;
}
