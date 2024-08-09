#include "game.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
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
void free_game_state(GameState *state);

void game(SDL_Renderer *renderer) {
    SDL_Event event;
    bool running = true;

    GameState state = {.board = create_board(8, 8, STARTING_FEN),
                       .screen_layout = create_screen_layout(),
                       .resources = load_resources(renderer),
                       .is_dragging = false,
                       .click_position = NULL};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

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

                    printf("Mouse down!\n");

                    break;
                case SDL_MOUSEBUTTONUP:
                    printf("Mouse up!\n");

                    if (!state.is_dragging) continue;


                    // After some testing it detects mouse ups even after leaving the window so this should just work 100% of the time
                    state.is_dragging = false;

                    break;
            }
        }

        // I think I could refactor this so I am not checking moust_state
        // multiple times per free_game_state Like only check during rendering
        // and just while it is down do true for is_dragging then only check the
        // threshold during rendering This actually might not work like how I
        // want because it would snap back to the original square when you got
        // near enough
        if (!state.is_dragging) {
            state.is_dragging = is_dragging(&state);
        }

        SDL_RenderClear(renderer);

        render_game_state(renderer, &state);

        SDL_RenderPresent(renderer);
    }

    free_game_state(&state);
}

void screen_to_board_cordinates(int world_x, int world_y, int *board_x,
                                int *board_y, const SDL_Rect *board) {
    world_x -= board->x;
    world_y -= board->y;

    (*board_x) = board->w / world_x;
    (*board_y) = board->h / world_y;
};

Resources *load_resources(SDL_Renderer *renderer) {
    Resources *resources = (Resources *)malloc(sizeof(Resources));
    resources->sprite_sheet =
        IMG_LoadTexture(renderer, get_path("../gfx/Pieces.png"));
    resources->square =
        IMG_LoadTexture(renderer, get_path("../gfx/Square.png"));

    resources->white = (SDL_Color){
        .r = 255,
        .g = 255,
        .b = 255,
    };

    resources->black = (SDL_Color){
        .r = 50,
        .g = 50,
        .b = 50,
    };

    return resources;
}

ScreenLayout *create_screen_layout() {
    ScreenLayout *layout = (ScreenLayout *)malloc(sizeof(ScreenLayout));

    layout->board_area.w = WINDOW_WIDTH * 0.75;
    layout->board_area.h = layout->board_area.w;
    layout->board_area.x = 15;
    layout->board_area.y = WINDOW_HEIGHT / 2 - layout->board_area.h / 2;

    return layout;
}

bool is_in_bounds(const SDL_Rect *rect, int x, int y);

void handle_click(int x, int y, GameState *game_state) {
    // Invert for guard clause
    // if the click was inside of the board
        // if square has piece
            // select piece
    ScreenLayout *layout = game_state->screen_layout;

    if (!is_in_bounds(&game_state->screen_layout->board_area, x, y)) {
        free(game_state->click_position);

        game_state->click_position = NULL;

        return;
    };

    int board_x = x;
    int board_y = y;

    board_x -= layout->board_area.x;
    board_y -= layout->board_area.y;

    board_x = layout->board_area.w / x;
    board_y = layout->board_area.h / y;

    // If empty square try make move 
    // click_position needs free and set to null
}

bool is_dragging(const GameState *state) {
    int mouse_position[2];

    int mouse_state = SDL_GetMouseState(&mouse_position[0], &mouse_position[1]);

    if (!(mouse_state & SDL_BUTTON_LMASK)) return false;

    int *click_position = state->click_position;

    if (click_position == NULL) return false;

    const int THRESHOLD = 15;

    int x_dif = click_position[0] - mouse_position[0];

    int y_dif = click_position[1] - mouse_position[1];

    float distance = sqrtf(x_dif * x_dif + y_dif * y_dif);

    return distance >= THRESHOLD;
}

SDL_Rect get_piece_src(SDL_Texture *spritesheet, Piece piece) {
    int width, height;

    SDL_QueryTexture(spritesheet, NULL, NULL, &width, &height);

    width /= 6;
    height /= 2;

    SDL_Rect src_rect = {.x = width * piece.type,
                         .y = height * piece.team,
                         .w = width,
                         .h = height};

    return src_rect;
}

// TODO: Finish the rendering of the selected piece
// right now it either isn't working or I'm not selecting a piece which it is
// probably the second one
void render_game_state(SDL_Renderer *renderer, const GameState *game_state) {
    Board *board = game_state->board;
    Resources *resources = game_state->resources;
    SDL_Texture *sprite_sheet = resources->sprite_sheet;
    ScreenLayout *layout = game_state->screen_layout;

    const int square_width = layout->board_area.w / board->width;
    const int square_height = layout->board_area.h / board->height;

    int *selected_piece = NULL;

    if (game_state->is_dragging && game_state->click_position) {
        selected_piece = (int *)malloc(sizeof(int) * 2);
        screen_to_board_cordinates(
            game_state->click_position[0], game_state->click_position[1],
            &selected_piece[0], &selected_piece[1], &layout->board_area);
    }

    for (int x = 0; x < game_state->board->width; x++) {
        for (int y = 0; y < game_state->board->height; y++) {
            SDL_Color color;

            if ((x + y) % 2 == 0) {
                color = game_state->resources->white;
            } else {
                color = game_state->resources->black;
            }

            SDL_SetTextureColorMod(resources->square, color.r, color.g,
                                   color.b);

            SDL_Rect rect = {.x = layout->board_area.x + square_width * x,
                             .y = layout->board_area.y + square_width * y,
                             .w = square_width,
                             .h = square_height};

            SDL_RenderCopy(renderer, resources->square, NULL, &rect);

            if (selected_piece) {
                if (selected_piece[0] == x && selected_piece[1] == y) continue;
            }

            Piece piece = board->grid[x + y * board->width];

            SDL_Rect piece_src = get_piece_src(sprite_sheet, piece);

            SDL_RenderCopy(renderer, sprite_sheet, &piece_src, &rect);
        }
    }

    free(selected_piece);
}

void free_game_state(GameState *state) {}
