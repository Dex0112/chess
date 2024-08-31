#ifndef BOARD_CONTEXT
#define BOARD_CONTEXT

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include "chess.h"

typedef struct {
    int squares;
    // I will have to figure out some sort of swapping algorithm to make sure
    // that all the pieces that are on the board stay in the front and bad
    // values go to the back, such as pieces that no longer exist
    int pieces;
    SDL_Color *square_colors;
    SDL_Rect *square_rects;
    SDL_Rect *piece_rects;
    SDL_Rect *piece_srcs;
    bool is_dragging;
    // this doesn't work with how I am currently doing this
    // SDL_Point *selected_piece_pos;
    int recompute_flags;
    int square_size;
    // previous move data
} BoardContext;

typedef struct {
    // Precompute the Src rects
    SDL_Rect *piece_srcs;
    SDL_Texture *pieces;
    SDL_Texture *square;
    SDL_Color white;
    SDL_Color black;
    SDL_Color light_highlight;
    SDL_Color dark_highlight;
} BoardResources;

typedef enum {
    RECOMPUTE_PIECE_RECTS = 1 << 0,
    RECOMPUTE_SQUARE_COLORS = 1 << 1,
    RECOMPUTE_SELECTED_PIECE = 1 << 2,
    // RECOMPUTE_SELECTED_PIECE,
    RECOMPUTE_ALL = 0xFFFFFFFF,
} BoardContextFlags;

void set_board_context_flag(BoardContext *, BoardContextFlags);

void process_board_context(const Board *board, BoardContext *context, const BoardResources *resources);

#endif
