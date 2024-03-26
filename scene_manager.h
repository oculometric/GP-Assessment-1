#pragma once

#include <chrono>

#include "vector2.h"
#include "mesh.h"
#include "object.h"
#include "game_manager.h"

class SceneManager
{
private:
	CameraObject* active_camera = NULL;

	Object* root_object;

	GameManager* game_manager;

	int last_mouse_x = 0;
	int last_mouse_y = 0;
	std::chrono::steady_clock::time_point last_frame_time;

	void renderFromCamera(CameraObject* camera);
	void renderAxesGizmo(CameraObject* camera);
	void renderHierarchy(Object* root);
	void drawObject(MeshObject* obj);
public:
	SceneManager(int argc, char* argv[], unsigned int x, unsigned int y, GameManager* game);

	// callbacks, don't touch these

	void display();
	void mouseMove(int x, int y);
	void mouseMovePassive(int x, int y);
	void mouseClick(int button, int state, int x, int y);
	void keyDown(uint8_t key, int x, int y);
	void keyUp(uint8_t key, int x, int y);
	void frameRefresh(int value);
	void resizeWindow(int x, int y);

	inline CameraObject* getCamera() { return active_camera; }
	void addObject(Object* obj);

	SceneManager() = delete;

	~SceneManager();
};