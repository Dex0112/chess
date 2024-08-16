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

#include "chess.h"
#include "clock.h"
#include "constants.h"
#include "path.h"

typedef struct {
    SDL_Texture *sprite_sheet;
    SDL_Texture *square;
    TTF_Font *clock_font;
    int font_size;
    SDL_Color white;
    SDL_Color black;
    SDL_Color selected;
    SDL_Color clock_active;
    SDL_Color clock_inactive;
    SDL_Color clock_text_active;
    SDL_Color clock_text_inactive;
} Resources;

typedef struct {
    SDL_Rect board;
    SDL_Rect white_clock;
    SDL_Rect black_clock;
} ScreenLayout;

typedef enum {
    STATE_IDLE,
    STATE_PIECE_SELECTED,
    STATE_DRAGGING_PIECE
} GameState;

// I screen_layout and resources should be const
typedef struct {
    Clock *clock;
    Board *board;
    const Resources resources;
    const ScreenLayout screen_layout;
    GameState game_state;
    SDL_Point selected;
} AppState;

// Should these return pointers?
// They should really just return the struct
Resources load_resources(SDL_Renderer *);
ScreenLayout create_screen_layout();

void handle_click(SDL_Point click, AppState *);

bool is_dragging(const AppState *);

void render_game_state(SDL_Renderer *renderer, const AppState *);
void free_game_state(AppState *state);

void game(SDL_Renderer *renderer) {
    TTF_Init();

    SDL_Event event;
    bool running = true;

    AppState state = {
        .board = create_board(8, 8, STARTING_FEN),
        .screen_layout = create_screen_layout(),
        .resources = load_resources(renderer),
        .game_state = STATE_IDLE,
        .selected = {-1, -1},
        .clock = (Clock *)malloc(sizeof(Clock)),
    };

    start_clock(state.clock, 15);

    int frame_count = 0;

    while (running) {
        clock_update(state.clock);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        clock_toggle(state.clock);
                        state.board->move_index++;
                    }

                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button != SDL_BUTTON_LEFT) continue;

                    SDL_Point click;
                    SDL_GetMouseState(&click.x, &click.y);

                    printf("Mouse down! (%d, %d)\n", click.x, click.y);

                    handle_click(click, &state);

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

        render_game_state(renderer, &state);

        SDL_RenderPresent(renderer);

        // SDL_Delay(50);
    }

    free_game_state(&state);
    TTF_Quit();
}

SDL_Point screen_to_board_cordinates(SDL_Point world, const AppState *state) {
    const SDL_Rect *board_area = &state->screen_layout.board;

    world.x -= board_area->x;
    world.y -= board_area->y;

    SDL_Point board_position;

    board_position.x = world.x;
    board_position.y = world.y;
    board_position.x /= (board_area->w / state->board->width);
    board_position.y /= (board_area->h / state->board->height);

    return board_position;
}

Resources load_resources(SDL_Renderer *renderer) {
    Resources resources = {
        .sprite_sheet =
            IMG_LoadTexture(renderer, get_path("../gfx/Pieces.png")),
        .square = IMG_LoadTexture(renderer, get_path("../gfx/Square.png")),
        .font_size = 40,
        .white = {.r = 255, .g = 255, .b = 255},
        .black = {.r = 50, .g = 50, .b = 50},
        .selected = {.r = 255, .g = 246, .b = 163},
        .clock_active = {.r = 255, .g = 255, .b = 255, .a = 255},
        .clock_inactive = {.r = 115, .g = 115, .b = 115, .a = 255},
    };

    resources.clock_font = TTF_OpenFont(
        "/usr/share/fonts/gnu-free/FreeSansBold.otf", resources.font_size);

    return resources;
}

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

bool is_in_bounds(const SDL_Rect *rect, int x, int y) {
    x -= rect->x;
    y -= rect->y;

    if (x < 0) return false;
    if (y < 0) return false;
    if (x >= rect->w) return false;
    if (y >= rect->h) return false;

    return true;
}

// Refactor this
void handle_click(SDL_Point click, AppState *state) {
    const ScreenLayout *layout = &state->screen_layout;

    if (!is_in_bounds(&layout->board, click.x, click.y)) {
        state->game_state = STATE_IDLE;

        return;
    }

    SDL_Point selected = screen_to_board_cordinates(click, state);

    if (state->game_state == STATE_PIECE_SELECTED) {
        if (state->selected.x == selected.x &&
            state->selected.y == selected.y) {
            state->game_state = STATE_IDLE;

            return;
        }
    }

    Piece piece = get_piece(state->board, selected.x, selected.y);

    if (piece.team == state->board->move_index % 2) {
        state->game_state = STATE_DRAGGING_PIECE;
        state->selected = selected;

        return;
    }

    if (state->game_state == STATE_PIECE_SELECTED) {
        // Trying to move
        printf("No move logic has been implemented yet!\n");
    }

    state->game_state = STATE_IDLE;
}

SDL_Rect get_piece_src(SDL_Texture *spritesheet, Piece piece) {
    const int PIECES = 6;
    const int TEAMS = 2;

    int width, height;

    SDL_QueryTexture(spritesheet, NULL, NULL, &width, &height);

    width /= PIECES;
    height /= TEAMS;

    SDL_Rect src_rect = {
        .x = width * piece.type,
        .y = height * piece.team,
        .w = width,
        .h = height,
    };

    return src_rect;
}

void render_game_state(SDL_Renderer *renderer, const AppState *state) {
    Board *board = state->board;
    const Resources *resources = &state->resources;
    SDL_Texture *sprite_sheet = resources->sprite_sheet;
    const ScreenLayout *layout = &state->screen_layout;

    const int square_width = layout->board.w / board->width;
    const int square_height = layout->board.h / board->height;

    for (int x = 0; x < state->board->width; x++) {
        for (int y = 0; y < state->board->height; y++) {
            SDL_Color color;

            // This might be too ambiguous
            bool selected_square = state->game_state != STATE_IDLE;

            if (selected_square) {
                selected_square =
                    x == state->selected.x && y == state->selected.y;
            }

            if (selected_square) {
                color = resources->selected;
            } else {
                if ((x + y) % 2 == 0) {
                    color = state->resources.white;
                } else {
                    color = state->resources.black;
                }
            }

            SDL_SetTextureColorMod(resources->square, color.r, color.g,
                                   color.b);

            SDL_Rect rect = {
                .x = layout->board.x + square_width * x,
                .y = layout->board.y + square_width * y,
                .w = square_width,
                .h = square_height,
            };

            SDL_RenderCopy(renderer, resources->square, NULL, &rect);

            if (state->game_state == STATE_DRAGGING_PIECE && selected_square) {
                continue;
            }

            Piece piece = get_piece(board, x, y);

            SDL_Rect piece_src = get_piece_src(sprite_sheet, piece);

            SDL_RenderCopy(renderer, sprite_sheet, &piece_src, &rect);
        }
    }

    SDL_Color white_clock_color, black_clock_color;
    SDL_Color white_text_color, black_text_color;

    double white_time, black_time;

    if (board->move_index % 2 == 0) {
        white_clock_color = resources->clock_active;
        black_clock_color = resources->clock_inactive;

        white_text_color = resources->clock_text_active;
        black_text_color = resources->clock_text_inactive;

        white_time = *state->clock->current;
        black_time = *state->clock->inactive;
    } else {
        white_clock_color = resources->clock_inactive;
        black_clock_color = resources->clock_active;

        white_text_color = resources->clock_text_inactive;
        black_text_color = resources->clock_text_active;

        white_time = *state->clock->inactive;
        black_time = *state->clock->current;
    }

    SDL_SetRenderDrawColor(renderer, white_clock_color.r, white_clock_color.g,
                           white_clock_color.b, white_clock_color.a);

    SDL_RenderFillRect(renderer, &state->screen_layout.white_clock);

    SDL_SetRenderDrawColor(renderer, black_clock_color.r, black_clock_color.g,
                           black_clock_color.b, black_clock_color.a);

    SDL_RenderFillRect(renderer, &state->screen_layout.black_clock);

    SDL_Rect text_rect = state->screen_layout.white_clock;

    char white_text[8], black_text[8];

    sprintf(white_text, "%.2f", white_time);
    sprintf(black_text, "%.2f", black_time);

    SDL_Surface *text_surface = TTF_RenderText_Blended(
        resources->clock_font, white_text, white_text_color);

    SDL_Texture *white_clock_text =
        SDL_CreateTextureFromSurface(renderer, text_surface);


    text_surface = TTF_RenderText_Blended(resources->clock_font, black_text,
                                          black_text_color);

    SDL_Texture *black_clock_text =
        SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_QueryTexture(white_clock_text, NULL, NULL, &text_rect.w, &text_rect.h);

    text_rect.x += (layout->white_clock.w - text_rect.w) / 2;
    text_rect.y += (layout->white_clock.h - text_rect.h) / 2;

    SDL_RenderCopy(renderer, white_clock_text, NULL, &text_rect);

    text_rect = layout->black_clock;
    SDL_QueryTexture(black_clock_text, NULL, NULL, &text_rect.w, &text_rect.h);

    text_rect.x += (layout->black_clock.w - text_rect.w) / 2;
    text_rect.y += (layout->black_clock.h - text_rect.h) / 2;

    SDL_RenderCopy(renderer, black_clock_text, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(white_clock_text);
    SDL_DestroyTexture(black_clock_text);

    if (state->game_state != STATE_DRAGGING_PIECE) return;

    Piece piece = get_piece(board, state->selected.x, state->selected.y);

    SDL_Rect rect = {.w = square_width, .h = square_height};
    SDL_Rect piece_src = get_piece_src(sprite_sheet, piece);

    // Lock this to the board area
    SDL_GetMouseState(&rect.x, &rect.y);

    rect.x -= rect.w / 2;
    rect.y -= rect.h / 2;

    SDL_RenderCopy(renderer, sprite_sheet, &piece_src, &rect);
}

void free_resources(Resources resources) {
    SDL_DestroyTexture(resources.sprite_sheet);
    SDL_DestroyTexture(resources.square);
}

// TODO:
void free_game_state(AppState *state) {
    free_board(state->board);
    free_resources(state->resources);
}
