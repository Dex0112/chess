#include "chess.h"

#include <ctype.h>
#include <malloc.h>
#include <string.h>

// Example fen
// "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
void load_fen(Board *board, char *fen) {
    Piece grid[board->width * board->height];

    for (int i = 0; i < board->width * board->height; i++) {
        grid[i].type = NONE;
    }

    int x = 0;
    int y = board->height - 1;
    for (int i = 0; i < strlen(fen); i++) {
        int index = x + y * board->height;
        char c = fen[i];

        if (isdigit(c)) {
            x += c - '0';
            continue;
        }

        if (c == '/') {
            y--;
            break;
        }

        if (c == ' ') {
            break;
        }

        PieceType piece_type = NONE;
        Team team = isupper(c) ? WHITE : BLACK;

        switch (tolower(c)) {
            case 'k':
                piece_type = KING;
                break;
            case 'q':
                piece_type = QUEEN;
                break;
            case 'r':
                piece_type = ROOK;
                break;
            case 'n':
                piece_type = KNIGHT;
                break;
            case 'b':
                piece_type = BISHOP;
                break;
            case 'p':
                piece_type = PAWN;
                break;
        };

        grid[index] = (Piece){
            piece_type,
            team,
        };
    }

    // Handel the rest of the fen string

    board->grid = grid;
}
