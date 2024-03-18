#pragma once

// forward declaration
class spacegame;

namespace glut_callback_handlers
{
	void init(spacegame* s);

	void display();

	void mouse_move(int x, int y);
	void mouse_move_passive(int x, int y);

	void mouse_click(int button, int state, int x, int y);
	
}