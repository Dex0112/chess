#ifndef CLOCK

#define CLOCK

#include <time.h>

typedef struct {
    double *current;
    double *inactive;

    clock_t last_tick;
} Clock;

void start_clock(Clock *chess_clock, int timer_seconds);

void clock_update(Clock *chess_clock);

// You should call clock_update before calling clock_toggle
void clock_toggle(Clock *chess_clock);
#endif
