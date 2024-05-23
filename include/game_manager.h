#pragma once

// forward declaration
class SceneManager;

// base class for games in the engine
class GameManager
{
public:
	// pointer to the scene manager actually running the show
	SceneManager* scene_manager;
	// should be overriden by subclasses and contain all code for loading resources and allocating memory
	virtual void init();
	// should be overriden by subclasses and contain code for actually loading the scene into the world
	virtual void start(SceneManager* manager);
	// should be overriden by subclasses and contain code to be run on each frame
	virtual void update(float delta_time);
	// optional to be overriden, called on input events

	virtual void mouseMove(int delta_x, int delta_y, bool down);
	virtual void mousePressed(int button, bool down, int pos_x, int pos_y);
	virtual void keyPressed(unsigned char key, bool down);
	// should be overriden by subclasses to undo everything done by start
	virtual void destroy();

	GameManager();
};