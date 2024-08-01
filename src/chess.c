#include "chess.h"

#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Example fen
// "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
void load_fen(Board *board, char *fen) {
    Piece *grid = (Piece *)malloc(board->width * board->height * sizeof(Piece));

    if (grid == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // Initialize the grid with empty pieces
    for (int i = 0; i < board->width * board->height; i++) {
        grid[i].type = NONE;
        grid[i].team = NONE;
    }

    int x = 0;
    int y = 0;
    int i = -1;

    while (fen[i] != ' ') {
        i++;

        int index = x + y * board->width;

        char c = fen[i];

        if (isdigit(c)) {
            x += c - '0';
            continue;
        }

        if (c == '/') {
            x = 0;
            y++;
            continue;
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
        }

        grid[index] = (Piece){piece_type, team};
        x++;
    }

    // use the index to check the rest of the strin

    board->grid = grid;
}

void get_coordinates(int x, int y, char *file, int *rank) {
    *(file) = 'a' + x;

    *(rank) = y + 1;
}

void free_board(Board *board) { free(board->grid); }
