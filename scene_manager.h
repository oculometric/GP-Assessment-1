#pragma once

#include <chrono>

#include "vector2.h"
#include "mesh.h"
#include "object.h"
#include "game_manager.h"
#include "texture.h"

class SceneManager
{
public:
	Texture* skybox = NULL;

private:
	CameraObject* active_camera = NULL;

	Object* root_object = NULL;

	GameManager* game_manager = NULL;

	LightObject lights[8];

	int last_mouse_x = 0;
	int last_mouse_y = 0;
	std::chrono::steady_clock::time_point last_frame_time;

	float* post_processing_buffer = NULL;
	int viewport_width = 0;
	int viewport_height = 0;

	void renderFromCamera(CameraObject* camera);
	void drawAxesGizmo(CameraObject* camera);
	void renderHierarchy(Object* root);
	void drawEnvironmentCubemap(CameraObject* camera);
	void drawObject(MeshObject* obj);
	void performPostProcessing(CameraObject* camera);
	void updateLights();
public:
	SceneManager(int argc, char* argv[], unsigned int x, unsigned int y, GameManager* game);
	SceneManager() = delete;

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
	inline LightObject* getLight(unsigned char index) { if (index >= 8) return NULL; else return lights + index; }

	~SceneManager();
};