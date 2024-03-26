#pragma once

#include <chrono>

#include "vector2.h"
#include "mesh.h"
#include "object.h"

class SpaceGame
{
private:
	CameraObject* active_camera = NULL;
	Vector3 camera_local_velocity;

	Object* root_object;

	Object* ship;

	int last_mouse_x = 0;
	int last_mouse_y = 0;
	std::chrono::steady_clock::time_point last_frame_time;

	void renderFromCamera(CameraObject* camera);
	void renderHierarchy(Object* root);
	void renderAxesGizmo(CameraObject* camera);
	void drawObject(MeshObject* obj);
public:
	SpaceGame(int argc, char* argv[], unsigned int x, unsigned int y);

	void display();
	void mouseMove(int x, int y);
	void mouseMovePassive(int x, int y);
	void mouseClick(int button, int state, int x, int y);
	void keyDown(uint8_t key, int x, int y);
	void keyUp(uint8_t key, int x, int y);
	void frameRefresh(int value);
	void resizeWindow(int x, int y);

	SpaceGame() = delete;

	~SpaceGame();
};