#include <stdlib.h>
#include <time.h>
#include "clock.h"

void start_clock(Clock *chess_clock, int timer_seconds) {
    chess_clock->last_tick = clock();
    chess_clock->current = (double *)malloc(sizeof(double));
    chess_clock->inactive = (double *)malloc(sizeof(double));

    *chess_clock->current = timer_seconds;
    *chess_clock->inactive = timer_seconds;
}

void clock_update(Clock *chess_clock) {
    clock_t time = clock();

    double delta = (double)(time - chess_clock->last_tick) / CLOCKS_PER_SEC;

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
