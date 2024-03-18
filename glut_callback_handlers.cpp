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

	void mouse_move(int x, int y) { if (game_global) game_global->mouse_move(x, y); }

	void mouse_move_passive(int x, int y) { if (game_global) game_global->mouse_move_passive(x, y); }

	void mouse_click(int button, int state, int x, int y) { if (game_global) game_global->mouse_click(button, state, x, y); }
}
