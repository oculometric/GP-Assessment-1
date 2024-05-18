#pragma once

// forward declaration
class SceneManager;

class GameManager
{
public:
	SceneManager* scene_manager;
	virtual void init();
	virtual void start(SceneManager* manager);
	virtual void update(float delta_time);
	virtual void mouseMove(int delta_x, int delta_y, bool down);
	virtual void mousePressed(int button, bool down, int pos_x, int pos_y);
	virtual void keyPressed(unsigned char key, bool down);

	GameManager();
};