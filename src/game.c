#include "game.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "chess.h"
#include "constants.h"
#include "path.h"

typedef struct {
    SDL_Texture *spritesheet;
    SDL_Texture *square;
    SDL_Color white;
    SDL_Color black;
} Resources;

// Idk if it would go in this struct but an SDL_Rect for the whole board area so
// it doesn't take up the whole screen

typedef struct {
    int x;
    int y;
} Vector;

typedef struct {
    Resources resources;
    Board board;
    bool is_dragging;
    Vector *start_drag;
    Vector *selected_piece;
} GameState;

bool is_dragging(const GameState *state);

void screen_to_board_coords(const Board *board, int x, int y, int *new_x,
                            int *new_y);

void render_game_state(SDL_Renderer *renderer, const GameState *game_data);

void free_game_state(GameState *);
void free_resources(Resources *);

void game(SDL_Renderer *renderer) {
    Resources resources = {
        .spritesheet = IMG_LoadTexture(renderer, get_path("../gfx/Pieces.png")),
        .square = IMG_LoadTexture(renderer, get_path("../gfx/Square.png")),
        .white = {255, 255, 255, 255},
        .black = {50, 50, 50, 255}};

    Board board = {
        .width = 8,
        .height = 8,
    };

    load_fen(&board, STARTING_FEN);

    GameState state = {.resources = resources,
                       .board = board,
                       .start_drag = NULL,
                       .selected_piece = NULL};

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // Make logic for if the mouse has moved so far while
                    // holding a piece it is considered dragging
                    if (state.start_drag) {
                        printf("How did we get here?\n");
                        free(state.start_drag);
                        state.start_drag = NULL;
                    }

                    int x, y;

                    SDL_GetMouseState(&x, &y);

                    state.start_drag = (Vector *)malloc(sizeof(Vector));

                    state.start_drag->x = x;
                    state.start_drag->y = y;

                    screen_to_board_coords(&board, x, y, &x, &y);

                    // select_piece(GameState *state, int x, int y)

                    free(state.selected_piece);
                    state.selected_piece = NULL;

                    if (state.board.grid[x + y * state.board.width].type !=
                        NONE) {
                        state.selected_piece = (Vector *)malloc(sizeof(Vector));

                        state.selected_piece->x = x;
                        state.selected_piece->y = y;
                    }

                    break;
                case SDL_MOUSEBUTTONUP:
                    free(state.selected_piece);
                    state.selected_piece = NULL;

                    state.is_dragging = false;

                    break;
            }
        }

        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK) {
            state.is_dragging = is_dragging(&state);
        }

        if (state.is_dragging) {
            printf("You are now draggin\n");
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render_game_state(renderer, &state);

        SDL_RenderPresent(renderer);

        // So it isn't running at an insane fps
        SDL_Delay(5);
    }

    free_game_state(&state);
}

bool is_dragging(const GameState *state) {
    // I have no idea if this number makes any sense
    const float threshold = 10;
    int x, y;

    SDL_GetMouseState(&x, &y);

    int x_dif = state->start_drag->x - x;
    int y_dif = state->start_drag->y - y;

    float distance = sqrtf(x_dif * x_dif + y_dif * y_dif);

    return distance > threshold;
}

void screen_to_board_coords(const Board *board, int x, int y, int *new_x,
                            int *new_y) {
    *new_x = x / (WINDOW_WIDTH / board->width);
    *new_y = y / (WINDOW_HEIGHT / board->height);
}

SDL_Rect get_piece_from_texture(SDL_Texture *spritesheet, Piece piece) {
    int cell_width;
    int cell_height;

    SDL_QueryTexture(spritesheet, NULL, NULL, &cell_width, &cell_height);

    cell_width /= 6;
    cell_height /= 2;

    SDL_Rect piece_src_rect = {cell_width * piece.type,
                               cell_height * piece.team, cell_width,
                               cell_height};

    return piece_src_rect;
}

// This needs refactored asap
void render_game_state(SDL_Renderer *renderer, const GameState *game_state) {
    // Destructuring of the game_state struct
    const Resources *resources = &game_state->resources;
    const SDL_Color *white = &game_state->resources.white;
    const SDL_Color *black = &game_state->resources.black;
    const Board *board = &game_state->board;

    const int cell_width = WINDOW_WIDTH / game_state->board.width;
    const int cell_height = WINDOW_HEIGHT / game_state->board.height;

    // Render the board
    for (int x = 0; x < board->width; x++) {
        for (int y = 0; y < board->height; y++) {
            const SDL_Color *square_color = (x + y) % 2 ? black : white;

            SDL_SetTextureColorMod(resources->square, square_color->r,
                                   square_color->g, square_color->b);

            SDL_Rect square_rect = {cell_width * x, cell_height * y, cell_width,
                                    cell_height};

            SDL_RenderCopy(renderer, resources->square, NULL, &square_rect);

            // Wowy this sucks
            if (game_state->selected_piece) {
                if (game_state->selected_piece->x == x) {
                    if (game_state->selected_piece->y == y) {
                        if (game_state->is_dragging) {
                            continue;
                        }
                    }
                }
            }

            SDL_Rect piece_src_rect = get_piece_from_texture(
                resources->spritesheet, board->grid[x + y * board->width]);

            SDL_Rect piece_rect = {cell_width * x, cell_height * y, cell_width,
                                   cell_height};

            SDL_RenderCopy(renderer, resources->spritesheet, &piece_src_rect,
                           &piece_rect);
        }
    }

    if (game_state->selected_piece == NULL) {
        return;
    }

    if (!game_state->is_dragging) {
        return;
    }

    SDL_Rect piece_src_rect = get_piece_from_texture(
        resources->spritesheet,
        board->grid[game_state->selected_piece->x +
                    game_state->selected_piece->y * board->width]);

    SDL_Rect piece_rect = {0, 0, cell_width, cell_height};

    SDL_GetMouseState(&piece_rect.x, &piece_rect.y);

    piece_rect.x -= piece_rect.w / 2;
    piece_rect.y -= piece_rect.h / 2;

    SDL_RenderCopy(renderer, resources->spritesheet, &piece_src_rect,
                   &piece_rect);
}

void free_resources(Resources *resources) {
    SDL_DestroyTexture(resources->spritesheet);
    SDL_DestroyTexture(resources->square);

    resources->spritesheet = NULL;
    resources->square = NULL;
}

void free_game_state(GameState *state) {
    free(state->start_drag);
    free(state->selected_piece);

    state->start_drag = NULL;
    state->selected_piece = NULL;

    free_resources(&state->resources);
}
