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
} Resources;

typedef struct {
    SDL_Rect board_area;
} ScreenLayout;

typedef enum { IDLE, PIECE_SELECTED, PIECE_DRAGGING } GameState;

// click_position
// selected_cell
typedef struct {
    Board *board;
    Resources *resources;
    ScreenLayout *screen_layout;
    GameState game_state;
    SDL_Point selected;
} AppState;

Resources *load_resources(SDL_Renderer *);
ScreenLayout *create_screen_layout();

void handle_click(int x, int y, AppState *);

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
        .game_state = IDLE,
        .selected = {0, 0},
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

                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    handle_click(x, y, &state);

                    break;
                case SDL_MOUSEBUTTONUP:
                    printf("Mouse up!\n");

                    // Temp: replace with trymove logic
                    if (state.game_state == PIECE_DRAGGING) {
                        state.game_state = IDLE;
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

void screen_to_board_cordinates(int world_x, int world_y, int *board_x,
                                int *board_y, const SDL_Rect *board_area,
                                const Board *board) {
    world_x -= board_area->x;
    world_y -= board_area->y;

    *board_x = world_x / (board_area->w / board->width);
    *board_y = world_y / (board_area->h / board->height);
}

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
void handle_click(int x, int y, AppState *state) {
    ScreenLayout *layout = state->screen_layout;

    if (!is_in_bounds(&layout->board_area, x, y)) {
        printf("This click was not on the board\n");

        // Temporary
        state->game_state = IDLE;

        // Unselect piece
        // Like switch game_state to handle that

        return;
    };

    int board_x, board_y;

    screen_to_board_cordinates(x, y, &board_x, &board_y, &layout->board_area,
                               state->board);

    printf("%d, %d\n", board_x, board_y);

    Piece selected_piece = get_piece(state->board, board_x, board_y);

    if (selected_piece.type == PIECE_NONE) {
        if (state->game_state == PIECE_SELECTED) {
            printf("TODO: Implement try move\n");

            // Only doing this while try move isn't impemented
            state->game_state = IDLE;
        }

        printf("Not selecting any piece\n");

        return;
    }

    if (selected_piece.team == state->board->turn_index % 2) {
        if (state->game_state == IDLE) {
            state->game_state = PIECE_DRAGGING;
        }

        state->selected.x = board_x;
        state->selected.y = board_y;

        return;
    }

    // I don't know what even the state would be to end up here
    printf("Why am I at the end of handle_click\n");
}

SDL_Rect get_piece_src(SDL_Texture *spritesheet, Piece piece) {
    const int PIECES = 6;
    const int TEAMS = 2;

    int width, height;

    SDL_QueryTexture(spritesheet, NULL, NULL, &width, &height);

    width /= PIECES;
    height /= TEAMS;

    SDL_Rect src_rect = {.x = width * piece.type,
                         .y = height * piece.team,
                         .w = width,
                         .h = height};

    return src_rect;
}

// TODO: Finish the rendering of the selected piece
// right now it either isn't working or I'm not selecting a piece which it is
// probably the second one
void render_game_state(SDL_Renderer *renderer, const AppState *state) {
    Board *board = state->board;
    Resources *resources = state->resources;
    SDL_Texture *sprite_sheet = resources->sprite_sheet;
    ScreenLayout *layout = state->screen_layout;

    const int square_width = layout->board_area.w / board->width;
    const int square_height = layout->board_area.h / board->height;

    for (int x = 0; x < state->board->width; x++) {
        for (int y = 0; y < state->board->height; y++) {
            SDL_Color color;

            if ((x + y) % 2 == 0) {
                color = state->resources->white;
            } else {
                color = state->resources->black;
            }

            SDL_SetTextureColorMod(resources->square, color.r, color.g,
                                   color.b);

            SDL_Rect rect = {.x = layout->board_area.x + square_width * x,
                             .y = layout->board_area.y + square_width * y,
                             .w = square_width,
                             .h = square_height};

            SDL_RenderCopy(renderer, resources->square, NULL, &rect);

            if (state->game_state == PIECE_DRAGGING) {
                if (x == state->selected.x && y == state->selected.y) continue;
            }

            Piece piece = get_piece(board, x, y);

            SDL_Rect piece_src = get_piece_src(sprite_sheet, piece);

            SDL_RenderCopy(renderer, sprite_sheet, &piece_src, &rect);
        }
    }

    if (state->game_state != PIECE_DRAGGING) return;
    Piece piece = get_piece(board, state->selected.x, state->selected.y);

    SDL_Rect rect = {.w = square_width, .h = square_height};
    SDL_Rect piece_src = get_piece_src(sprite_sheet, piece);

    SDL_GetMouseState(&rect.x, &rect.y);

    rect.x -= rect.w / 2;
    rect.y -= rect.h / 2;

    SDL_RenderCopy(renderer, sprite_sheet, &piece_src, &rect);
}

void free_game_state(AppState *state) {}
