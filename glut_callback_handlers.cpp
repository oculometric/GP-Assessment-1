#include "glut_callback_handlers.h"

#include "spacegame.h"

namespace glut_callback_handlers
{
	namespace
	{
		SpaceGame* game_global = nullptr;
	}

	void init(SpaceGame* s) { game_global = s; }

	void display() { if (game_global) game_global->display(); }

	void mouseMove(int x, int y) { if (game_global) game_global->mouseMove(x, y); }

	void mouseMovePassive(int x, int y) { if (game_global) game_global->mouseMovePassive(x, y); }

	void mouseClick(int button, int state, int x, int y) { if (game_global) game_global->mouseClick(button, state, x, y); }

	void keyDown(uint8_t key, int x, int y) { if (game_global) game_global->keyDown(key, x, y); }

	void keyUp(uint8_t key, int x, int y) { if (game_global) game_global->keyUp(key, x, y); }

	void frameRefresh(int value) { if (game_global) game_global->frameRefresh(value); }

}
