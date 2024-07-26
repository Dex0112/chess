#ifndef CHESS
#define CHESS

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
    // Position for en passent
    // bool white_castle_q (queen side)
    // bool white_castle_k (king side)
    Piece *grid;
} Board;

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8

// Load this but as a file and not as code 
// https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

// Implement support for chess notation
// Fenstrings
// Implement support for undo/redo moves
// I want to implement an AI api kinda deal so a generic make move function
// Don't be afraid to look algorithms and common practices up while coding

// Edits the board
void load_fen(Board *board, char *fen);

char *generate_fen(Board);

// Make move
// Get moves
// is in check(team)
#endif
