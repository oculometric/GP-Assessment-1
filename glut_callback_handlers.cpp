#include "glut_callback_handlers.h"

#include "spacegame.h"

namespace glut_callback_handlers
{
	namespace
	{
		spacegame* game_global = nullptr;
	}

	void init(spacegame* s) { game_global = s; }

	void display() { if (game_global) game_global->display(); }
}