#ifndef CHESS
#define CHESS

#include <stdbool.h>

typedef enum {
    WHITE = 0,
    BLACK = 1,
} Team;

typedef enum {
    NONE = -1,
    KING = 0,
    QUEEN = 1,
    BISHOP = 2,
    KNIGHT = 3,
    ROOK = 4,
    PAWN = 5,
} PieceType;

typedef struct {
    PieceType type;
    Team team;
} Piece;

typedef struct {
    unsigned int width;
    unsigned int height;

    bool white_castle_q;
    bool white_castle_k;
    bool black_castle_q;
    bool black_castle_k;

    Team turn;
    // Position for en passent

    // Since or captur or pawn move
    // int half_moves;

    // This is full moves and increments after blacks moves
    // int move_index;

    Piece *grid;
} Board;

typedef struct move_data {
    Piece *target_piece;
    Piece *taken_piece;
    // Used for castling
    struct move_data *additional_move;
} MoveData;

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8

// https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

// Fenstrings
// Implement support for undo/redo moves
// I want to implement an AI api kinda deal so a generic make move function
// Don't be afraid to look algorithms and common practices up while coding

// Edits the board
void load_fen(Board *board, char *fen);

char *generate_fen(Board);

void free_board(Board *board);

void get_coordinates(int x, int y, char *file, int *rank);

// Make move
// Get moves
// is in check(team)
#endif
