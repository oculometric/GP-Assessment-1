#pragma once

#include "game_manager.h"

#define NUM_GAMES 2

void setGame(size_t index, GameManager* game);

GameManager* nextGame();