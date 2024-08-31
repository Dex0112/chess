// time for rewrite v2

#include "game.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "board_context.h"
#include "chess.h"
#include "clock.h"
#include "constants.h"

// Put this in a reasonable spot later
// I also am conflicted on whether to keep this as a struct or not
ScreenLayout create_screen_layout() {
    ScreenLayout layout;

    const int VERTICAL_CLOCK_PADDING = 16;

    layout.board.w = WINDOW_WIDTH * 0.75;
    layout.board.h = layout.board.w;
    layout.board.x = 15;
    layout.board.y = WINDOW_HEIGHT / 2 - layout.board.h / 2;

    layout.white_clock.w = 120;
    layout.white_clock.h = 50;
    layout.white_clock.x = layout.board.x + layout.board.w;
    layout.white_clock.x +=
        (WINDOW_WIDTH - layout.board.w - layout.board.x) / 2;
    layout.white_clock.x -= layout.white_clock.w / 2;
    layout.white_clock.y = WINDOW_HEIGHT / 2;

    layout.black_clock = layout.white_clock;

    layout.white_clock.y += VERTICAL_CLOCK_PADDING / 2;

    layout.black_clock.y -= layout.black_clock.h;
    layout.black_clock.y -= VERTICAL_CLOCK_PADDING / 2;

    return layout;
}

// void handle_click(SDL_Point click, AppState *);

void game(SDL_Renderer *renderer) {
    TTF_Init();

    SDL_Event event;
    bool running = true;

    AppState state = {
        .board = create_board(8, 8, STARTING_FEN),
        .game_state = STATE_IDLE,
        .selected = {-1, -1},
        .clock = (Clock *)malloc(sizeof(Clock)),
    };

    BoardContext ctx;

    set_board_context_flag(&ctx, RECOMPUTE_ALL);
    set_board_context_flag(&ctx, RECOMPUTE_PIECE_RECTS);
    set_board_context_flag(&ctx, RECOMPUTE_SQUARE_COLORS);
    set_board_context_flag(&ctx, RECOMPUTE_SELECTED_PIECE);

    process_board_context(state.board, &ctx);

    return;

    start_clock(state.clock, 15);

    int frame_count = 0;

    render_init(renderer);

    while (running) {
        clock_update(state.clock);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                // This is just for testing
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        clock_toggle(state.clock);
                        state.board->move_index++;
                    }

                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button != SDL_BUTTON_LEFT) continue;

                    SDL_Point click = {
                        .x = event.button.x,
                        .y = event.button.y,
                    };

                    printf("Mouse down! (%d, %d)\n", click.x, click.y);

                    // TODO: Implement some kind of input handling
                    // handle_click(click, &state);

                    break;
                case SDL_MOUSEBUTTONUP:
                    printf("Mouse up!\n");

                    // Temp: replace with trymove logic
                    if (state.game_state == STATE_DRAGGING_PIECE) {
                        state.game_state = STATE_PIECE_SELECTED;
                    }

                    break;
            }
        }

        // I don't need to render every frame just when something changes and I
        // need to implement that
        // Ex: dragging or showing available moves
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Preprocess board
        // check if there is any reason to actually render anything
        // Render board

        SDL_RenderPresent(renderer);

        // SDL_Delay(50);
    }

    TTF_Quit();
}

// Refactor this
// Also find where it belongs
// void handle_click(SDL_Point click, AppState *state) {
//     const ScreenLayout *layout = &state->screen_layout;
//
//     if (!is_in_bounds(&layout->board, click.x, click.y)) {
//         state->game_state = STATE_IDLE;
//
//         return;
//     }
//
//     SDL_Point selected = screen_to_board_cordinates(click, state);
//
//     if (state->game_state == STATE_PIECE_SELECTED) {
//         if (state->selected.x == selected.x &&
//             state->selected.y == selected.y) {
//             state->game_state = STATE_IDLE;
//
//             return;
//         }
//     }
//
//     Piece piece = get_piece(state->board, selected.x, selected.y);
//
//     if (piece.team == state->board->move_index % 2) {
//         state->game_state = STATE_DRAGGING_PIECE;
//         state->selected = selected;
//
//         return;
//     }
//
//     if (state->game_state == STATE_PIECE_SELECTED) {
//         // Trying to move
//         printf("No move logic has been implemented yet!\n");
//     }
//
//     state->game_state = STATE_IDLE;
// }
