#pragma once

#include "game_manager.h"

#define NUM_GAMES 2

// assign a game into the array
void setGame(size_t index, GameManager* game);

// advance to the next game manager. used for switching scenes
GameManager* nextGame();