#include "board_context.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "chess.h"
#include "path.h"

// Maybe have functions for all the actions you would want to do like switch
// last move data Ex: update_previous_move set_selected_piece set_is_dragging
// and such so that I can set the flags and the person calling these functions
// never needs to worry about setting flags

SDL_Rect get_piece_src(const BoardResources *, Piece piece);

void set_board_context_flag(BoardContext *context, BoardContextFlags flag) {
    context->recompute_flags |= flag;
}

// TODO: test this function
// Maybe return true to see if rerendering is needed
void process_board_context(const Board *board, BoardContext *context,
                           const BoardResources *resources) {
    // There is nothing to do
    if (context->recompute_flags == 0) return;

    if (context->recompute_flags & RECOMPUTE_PIECE_RECTS) {
        for (int x = 0; x < board->width; x++) {
            for (int y = 0; y < board->height; y++) {
                Piece piece = get_piece(board, x, y);

                if (piece.type == PIECE_NONE) continue;
            }
        }
        printf("Recomputing piece rects\n");
    }

    if (context->recompute_flags & RECOMPUTE_SQUARE_COLORS) {
        printf("Recomputing square colors\n");
    }

    if (context->recompute_flags & RECOMPUTE_SELECTED_PIECE) {
        printf("Recomputing Selected Piece\n");
    }

    context->recompute_flags = 0;
}

// TODO: Implement the resources part into this file
BoardResources load_default_board_resources(SDL_Renderer *renderer) {
    BoardResources resources = {
        .piece_srcs =
            (SDL_Rect *)malloc(sizeof(SDL_Rect) * (PIECE_PAWN + 1) * 2),
        .pieces = IMG_LoadTexture(renderer, get_path("../gfx/Pieces.png")),
        .square = IMG_LoadTexture(renderer, get_path("../gfx/Square.png")),
        .white = {.r = 255, .g = 255, .b = 255},
        .black = {.r = 50, .g = 50, .b = 50},
        .light_highlight = {.r = 255, .g = 246, .b = 163},
    };

    // PIECE_PAWN is the last element listed in the enum therefore it is length
    // of the pieces

    for (int i = 0; i <= PIECE_PAWN; i++) {
        Piece piece = {
            .type = (PieceType)i,
            .team = TEAM_WHITE,
        };

        // load the src rects
    }

    return resources;
}

// I am going to need to refactor this whole thing when I refactor how I handle
// pieces
SDL_Rect get_piece_src(const BoardResources *resources, Piece piece) {}
