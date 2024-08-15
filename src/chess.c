#include "chess.h"

#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Board *create_board(int width, int height, char *fen) {
    Board *board = (Board *)malloc(sizeof(Board));

    board->width = width;
    board->height = height;
    board->move_index = 0;

    if (!fen) {
        board->grid = (Piece *)malloc(width * height * sizeof(Piece));

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < width; y++) {
                board->grid[x + y * width] = (Piece) {
                    .type = PIECE_NONE
                };
            }
        }

        return board;
    }

    load_fen(board, fen);

    return board;
}

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
        grid[i].type = PIECE_NONE;
        grid[i].team = PIECE_NONE;
    }

    int x = 0;
    int y = 0;
    int i = -1;

    // Rewrite as a for loop with strlen
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

        PieceType piece_type = PIECE_NONE;
        Team team = isupper(c) ? TEAM_WHITE : TEAM_BLACK;

        switch (tolower(c)) {
            case 'k':
                piece_type = PIECE_KING;
                break;
            case 'q':
                piece_type = PIECE_QUEEN;
                break;
            case 'r':
                piece_type = PIECE_ROOK;
                break;
            case 'n':
                piece_type = PIECE_KNIGHT;
                break;
            case 'b':
                piece_type = PIECE_BISHOP;
                break;
            case 'p':
                piece_type = PIECE_PAWN;
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

Piece get_piece(const Board *board, int x, int y) {
    int index = x + y * board->width;

    return board->grid[index];
}

void free_board(Board *board) { free(board->grid); }
