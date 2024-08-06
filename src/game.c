// This file is due for a refactor immediately
// I will probably do this tomorrow before reinstalling my system

#include "game.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>

#include "chess.h"
#include "constants.h"
#include "path.h"

// Idk if it would go in this struct but an SDL_Rect for the whole board area so
// it doesn't take up the whole screen

typedef struct {
} GameState;

bool is_dragging(const GameState *state);

void render_game_state(SDL_Renderer *renderer, const GameState *game_data);

void game(SDL_Renderer *renderer) {}

bool is_dragging(const GameState *state) { return false; }

void render_game_state(SDL_Renderer *renderer, const GameState *game_state) {}

void free_game_state(GameState *state) {}
