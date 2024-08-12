// time for rewrite v2

#include "game.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "chess.h"
#include "constants.h"
#include "path.h"

typedef struct {
    SDL_Texture *sprite_sheet;
    SDL_Texture *square;
    SDL_Color white;
    SDL_Color black;
    SDL_Color selected;
} Resources;

typedef struct {
    SDL_Rect board_area;
} ScreenLayout;

typedef enum {
    STATE_IDLE,
    STATE_PIECE_SELECTED,
    STATE_DRAGGING_PIECE
} GameState;

// I screen_layout and resources should be const
typedef struct {
    Board *board;
    Resources resources;
    ScreenLayout screen_layout;
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
    SDL_Event event;
    bool running = true;

    AppState state = {
        .board = create_board(8, 8, STARTING_FEN),
        .screen_layout = create_screen_layout(),
        .resources = load_resources(renderer),
        .game_state = STATE_IDLE,
        .selected = {-1, -1},
    };

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button != SDL_BUTTON_LEFT) continue;
                    printf("Mouse down!\n");

                    SDL_Point click;
                    SDL_GetMouseState(&click.x, &click.y);

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
        SDL_RenderClear(renderer);

        render_game_state(renderer, &state);

        SDL_RenderPresent(renderer);
    }

    free_game_state(&state);
}

SDL_Point screen_to_board_cordinates(SDL_Point world, const AppState *state) {
    const SDL_Rect *board_area = &state->screen_layout.board_area;

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
    Resources resources = {};
    resources.sprite_sheet =
        IMG_LoadTexture(renderer, get_path("../gfx/Pieces.png"));
    resources.square =
        IMG_LoadTexture(renderer, get_path("../gfx/Square.png"));

    resources.white = (SDL_Color){
        .r = 255,
        .g = 255,
        .b = 255,
    };

    resources.black = (SDL_Color){
        .r = 50,
        .g = 50,
        .b = 50,
    };
    // Make this like yellow or something

    resources.selected = (SDL_Color){
        .r = 255,
        .g = 246,
        .b = 163,
    };

    return resources;
}

ScreenLayout create_screen_layout() {
    ScreenLayout layout;

    layout.board_area.w = WINDOW_WIDTH * 0.75;
    layout.board_area.h = layout.board_area.w;
    layout.board_area.x = 15;
    layout.board_area.y = WINDOW_HEIGHT / 2 - layout.board_area.h / 2;

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

    if (!is_in_bounds(&layout->board_area, click.x, click.y)) {
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

    if (piece.team == state->board->turn_index % 2) {
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

    const int square_width = layout->board_area.w / board->width;
    const int square_height = layout->board_area.h / board->height;

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
                .x = layout->board_area.x + square_width * x,
                .y = layout->board_area.y + square_width * y,
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

    if (state->game_state != STATE_DRAGGING_PIECE) return;

    Piece piece = get_piece(board, state->selected.x, state->selected.y);

    SDL_Rect rect = {.w = square_width, .h = square_height};
    SDL_Rect piece_src = get_piece_src(sprite_sheet, piece);

    SDL_GetMouseState(&rect.x, &rect.y);

    rect.x -= rect.w / 2;
    rect.y -= rect.h / 2;

    SDL_RenderCopy(renderer, sprite_sheet, &piece_src, &rect);
}

// TODO:
void free_game_state(AppState *state) {}
