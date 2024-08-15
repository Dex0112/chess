#ifndef CHESS
#define CHESS

#include <stdbool.h>

typedef enum {
    TEAM_WHITE = 0,
    TEAM_BLACK = 1,
} Team;

typedef enum {
    PIECE_NONE = -1,
    PIECE_KING = 0,
    PIECE_QUEEN = 1,
    PIECE_BISHOP = 2,
    PIECE_KNIGHT = 3,
    PIECE_ROOK = 4,
    PIECE_PAWN = 5,
} PieceType;

typedef struct {
    PieceType type;
    Team team;
    // bool for has moved
} Piece;

typedef struct {
    unsigned int width;
    unsigned int height;
    // Half turns
    unsigned int move_index;

    Piece *grid;
} Board;

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8

// https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

Board *create_board(int width, int height, char *fen);

void load_fen(Board *board, char *fen);

Piece get_piece(const Board *, int x, int y);

void free_board(Board *board);
#endif
