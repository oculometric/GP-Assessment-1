#include "multi_game.h"

// static array of game managers for different scenes
GameManager* games[NUM_GAMES] = { nullptr };
size_t current_game = 0;

void setGame(size_t index, GameManager* game)
{
	if (index >= NUM_GAMES) return;
	games[index] = game;
}

GameManager* nextGame()
{
	while (true)
	{
		current_game++;
		if (current_game >= NUM_GAMES) current_game = 0;
		if (games[current_game] != nullptr) return games[current_game];
	}
}