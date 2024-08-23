#include "render.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

#include "chess.h"
#include "constants.h"
#include "game.h"
#include "path.h"

typedef struct {
    SDL_Texture *sprite_sheet;
    SDL_Texture *square;
    TTF_Font *clock_font;
    int font_size;
    SDL_Color white;
    SDL_Color black;
    SDL_Color selected;
    SDL_Color clock_active;
    SDL_Color clock_inactive;
    SDL_Color clock_text_active;
    SDL_Color clock_text_inactive;
} Resources;

typedef struct {
    // Should this be a board point with a board_size
    SDL_Rect board;
    // Refactor to have a clock x width and height
    // then a white y and a black y
    // To even further reduce code duplication
    // I belive SDL_Rect to be the incorrect abstraction
    SDL_Rect white_clock;
    SDL_Rect black_clock;
} ScreenLayout;

Resources load_resources(SDL_Renderer *renderer);
ScreenLayout create_screen_layout();

Resources resources;
ScreenLayout screen_layout;

void render_init(SDL_Renderer *renderer) {
    resources = load_resources(renderer);
    screen_layout = create_screen_layout();
}

Resources load_resources(SDL_Renderer *renderer) {
    Resources resources = {
        .sprite_sheet =
            IMG_LoadTexture(renderer, get_path("../gfx/Pieces.png")),
        .square = IMG_LoadTexture(renderer, get_path("../gfx/Square.png")),
        .font_size = 40,
        .white = {.r = 255, .g = 255, .b = 255},
        .black = {.r = 50, .g = 50, .b = 50},
        .selected = {.r = 255, .g = 246, .b = 163},
        .clock_active = {.r = 255, .g = 255, .b = 255, .a = 255},
        .clock_inactive = {.r = 115, .g = 115, .b = 115, .a = 255},
    };

    resources.clock_font = TTF_OpenFont(
        "/usr/share/fonts/gnu-free/FreeSansBold.otf", resources.font_size);

    return resources;
}

ScreenLayout create_screen_layout() {
    ScreenLayout layout;

    const int VERTICAL_CLOCK_PADDING = 16;

    layout.board.w = WINDOW_WIDTH * 0.75;
    layout.board.h = layout.board.w;
    layout.board.x = 15;
    layout.board.y = WINDOW_HEIGHT / 2 - layout.board.h / 2;

    layout.white_clock.w = 120;
    layout.white_clock.h = 50;
    layout.white_clock.x = layout.board.x + layout.board.w;
    layout.white_clock.x +=
        (WINDOW_WIDTH - layout.board.w - layout.board.x) / 2;
    layout.white_clock.x -= layout.white_clock.w / 2;
    layout.white_clock.y = WINDOW_HEIGHT / 2;

    layout.black_clock = layout.white_clock;

    layout.white_clock.y += VERTICAL_CLOCK_PADDING / 2;

    layout.black_clock.y -= layout.black_clock.h;
    layout.black_clock.y -= VERTICAL_CLOCK_PADDING / 2;

    return layout;
}

// I am going to need to refactor this whole thing when I refactor how I handle
// pieces
void render_piece(SDL_Renderer *renderer, SDL_Texture *sprite_sheet,
                  Piece piece, const SDL_Rect *rect) {
    const int PIECE_COUNT = 6;
    const int TEAMS = 2;
    int cell_width, cell_height;

    SDL_QueryTexture(sprite_sheet, NULL, NULL, &cell_width, &cell_height);

    cell_width /= PIECE_COUNT;
    cell_height /= TEAMS;

    SDL_Rect src = {
        .x = cell_width * piece.type,
        .y = cell_height * piece.team,
        .w = cell_width,
        .h = cell_height,
    };

    SDL_RenderCopy(renderer, sprite_sheet, &src, rect);
}

// Should this function handle getting the highlighted colors aswell
// I think maybe
SDL_Color get_square_color(int x, int y) {
    if ((x + y) % 2 == 0) return resources.white;

    return resources.black;
}

// Parameter for last move
void render_board(SDL_Renderer *renderer, const Board *board,
                  const SDL_Point *selected) {
    const int SQUARE_SIZE = screen_layout.board.w / board->width;

    for (int x = 0; x < board->width; x++) {
        for (int y = 0; y < board->height; y++) {
            SDL_Color color = get_square_color(x, y);

            SDL_SetTextureColorMod(resources.square, color.r, color.g, color.g);

            SDL_Rect square_rect = {
                .x = screen_layout.board.x + SQUARE_SIZE * x,
                .y = screen_layout.board.y + SQUARE_SIZE * y,
                .w = SQUARE_SIZE,
                .h = SQUARE_SIZE,
            };

            SDL_RenderCopy(renderer, resources.square, NULL, &square_rect);

            Piece piece = get_piece(board, x, y);

            if (piece.type == PIECE_NONE) continue;

            if (selected && selected->x == x && selected->y == y) continue;

            render_piece(renderer, resources.sprite_sheet, piece, &square_rect);
        }
    }

    if (!selected) return;

    Piece selected_piece = get_piece(board, selected->x, selected->y);

    SDL_Rect piece_rect = {
        .w = SQUARE_SIZE,
        .h = SQUARE_SIZE,
    };

    if (!(SDL_GetMouseState(&piece_rect.x, &piece_rect.y) & SDL_BUTTON_LMASK)) {
        piece_rect.x = selected->x * SQUARE_SIZE;
        piece_rect.y = selected->y * SQUARE_SIZE;
    }

    // If you are dragging make sure that the piece is locked within the bounds of the board

    render_piece(renderer, resources.sprite_sheet, selected_piece, &piece_rect);
}
