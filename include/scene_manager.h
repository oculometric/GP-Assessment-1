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
	// texture used for the skybox
	Texture* skybox = NULL;

private:
	// camera from which the scene is rendered from (or not at all if null)
	CameraObject* active_camera = NULL;

	// root objects for the overlay and the rest of the world

	Object* world_root = NULL;
	Object* overlay_root = NULL;

	// current game to which update events are passed

	GameManager* game_manager = NULL;

	// array of light objects
	LightObject lights[8];

	// mouse position on the last frame

	int last_mouse_x = 0;
	int last_mouse_y = 0;

	// chrono time at the start of the last frame
	std::chrono::steady_clock::time_point last_frame_time;

	// buffer for storing the screen during post-processing (stupid hack)
	float* post_processing_buffer = NULL;
	// texture Id for the post-processing buffer when it's being sent back to the GPU (other part of the stupid hack)
	unsigned int post_process_texture_id = -1;
	
	// size of the viewport

	int viewport_width = 0;
	int viewport_height = 0;

	// flag for skipping the post-processing step
	bool skip_postprocess = false;

	// flag for enabling debug view (shows object origins, wireframes, bounding boxes)
	bool debug_view = false;

	// look-up-table texture
	Vector3* lut_buffer = NULL;

	// top-level function that performs a frame of rendering the world
	void renderFromCamera(CameraObject* camera);
	// render overlay objects
	void drawOverlay(CameraObject* camera);
	// render an object and its children (recursive)
	void renderHierarchy(Object* root);
	// render the environment cubemap
	void drawEnvironmentCubemap(CameraObject* camera);
	// draw a mesh object (assumes transforms have already been applied
	void drawObject(MeshObject* obj);
	// draw debug info for an object (origin, axes, bounding box if mesh)
	void drawObjectDebug(Object* obj);
	// draw a particle object (assumes transforms have already been applied)
	void drawParticle(ParticleObject* obj);
	// perform post-processing on the framebuffer. has a massive performance impact
	void performPostProcessing(CameraObject* camera);
	// update GL lights from our light objects
	void updateLights();

	// counters for tracking scene statistics

	unsigned int objects_drawn_last_frame = 0;
	unsigned int triangles_drawn_last_frame = 0;
	float current_fps = 0;
public:
	// default initialiser
	SceneManager();

	// configure GLUT and OpenGL, as well as this class
	void initialise(int argc, char* argv[], unsigned int x, unsigned int y);
	// pass control to GLUT, specifying a game to run with
	void startMainloop(GameManager* game);

	// callbacks, only glut_callback_handlers should ever touch these

	void display();
	void mouseMove(int x, int y);
	void mouseMovePassive(int x, int y);
	void mouseClick(int button, int state, int x, int y);
	void keyDown(uint8_t key, int x, int y);
	void keyUp(uint8_t key, int x, int y);
	void frameRefresh(int value);
	void resizeWindow(int x, int y);
	void togglePostprocess();
	void menuAction(int index);

	// return the active camera pointer
	inline CameraObject* getCamera() { return active_camera; }
	// add an object to the scene root
	void addObject(Object* obj);
	// add an object to the overlay root
	void addOverlayObject(Object* obj);
	// get a pointer to a light object
	inline LightObject* getLight(unsigned char index) { if (index >= 8) return NULL; else return lights + index; }
	
	// set the currently referenced game
	void setGameManager(GameManager* game);
	// set the currently rendering camera (must be a child of the scene)
	inline void setCamera(CameraObject* new_camera) { active_camera = new_camera; }

	~SceneManager();
};