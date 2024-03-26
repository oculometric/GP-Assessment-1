#pragma once

#include <stdint.h>

// forward declaration
class SceneManager;

namespace glut_callback_handlers
{
	void init(SceneManager* s);

	void display();

	void mouseMove(int x, int y);
	void mouseMovePassive(int x, int y);

	void mouseClick(int button, int state, int x, int y);
	
	void keyDown(uint8_t key, int x, int y);
	void keyUp(uint8_t key, int x, int y);

	void frameRefresh(int value);

	void resizeWindow(int x, int y);
}