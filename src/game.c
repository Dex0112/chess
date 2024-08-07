// This file is due for a refactor immediately
// I will probably do this tomorrow before reinstalling my system

#include "game.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdlib.h>

#include "chess.h"
#include "constants.h"
#include "path.h"

typedef struct {
    SDL_Texture *pieces_sheet;
    SDL_Texture *square;
    SDL_Color white;
    SDL_Color black;
} Resources;

typedef struct {
    SDL_Rect board_area;
} ScreenLayout;

typedef struct {
    Board *board;
    Resources *resources;
    ScreenLayout *screen_layout;
    bool is_dragging;
    // This is an array
    int *click_position;
} GameState;

Resources *load_resources(SDL_Renderer *);
ScreenLayout *create_screen_layout();

void handle_click(int x, int y, GameState *);

bool is_dragging(const GameState *);

void render_game_state(SDL_Renderer *renderer, const GameState *);

void game(SDL_Renderer *renderer) {
    SDL_Event event;
    bool running = false;

    GameState state = {.board = create_board(8, 8, STARTING_FEN),
                       .screen_layout = create_screen_layout(),
                       .resources = load_resources(renderer),
                       .is_dragging = false,
                       .click_position = NULL};

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button != SDL_BUTTON_LEFT) continue;

                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    handle_click(x, y, &state);

                    break;
                case SDL_MOUSEBUTTONUP:
                    if (!state.is_dragging) continue;

                    state.is_dragging = false;
                    // Type to make the move
                    // Free click_position maybe

                    break;
            }
        }

        render_game_state(renderer, &state);
    }
}

Resources *load_resources(SDL_Renderer *renderer) {
    Resources *resources = (Resources *)malloc(sizeof(Resources));
    resources->pieces_sheet =
        IMG_LoadTexture(renderer, get_path("../gfx/Pieces.png"));
    resources->square =
        IMG_LoadTexture(renderer, get_path("../gfx/Square.png"));

    resources->white = (SDL_Color){
        .r = 255,
        .g = 255,
        .b = 255,
    };

    resources->black = (SDL_Color){
        .r = 175,
        .g = 175,
        .b = 175,
    };

    return NULL;
}

ScreenLayout *create_screen_layout() {
    ScreenLayout *layout = (ScreenLayout *)malloc(sizeof(ScreenLayout));

    layout->board_area.w = WINDOW_WIDTH * 0.5;
    layout->board_area.h = layout->board_area.w;
    layout->board_area.x = 15;
    layout->board_area.y = WINDOW_HEIGHT / 2 - layout->board_area.h / 2;

    return layout;
}

void handle_click(int x, int y, GameState *game_state) {}

bool is_dragging(const GameState *state) { return false; }

void render_game_state(SDL_Renderer *renderer, const GameState *game_state) {}

void free_game_state(GameState *state) {}
