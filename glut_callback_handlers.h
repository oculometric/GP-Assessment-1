#pragma once

#include <stdint.h>

// forward declaration
class spacegame;

namespace glut_callback_handlers
{
	void init(spacegame* s);

	void display();

	void mouse_move(int x, int y);
	void mouse_move_passive(int x, int y);

	void mouse_click(int button, int state, int x, int y);
	
	void key_down(uint8_t key, int x, int y);
	void key_up(uint8_t key, int x, int y);

	void frame_refresh(int value);
}