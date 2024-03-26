#pragma once

// forward declaration
class SceneManager;

class GameManager
{
protected:
	SceneManager* scene_manager;
public:
	virtual void init(SceneManager* manager);
	virtual void start();
	virtual void update(float delta_time);
	virtual void mouseMove(int delta_x, int delta_y, bool down);
	virtual void mousePressed(int button, bool down, int pos_x, int pos_y);
	virtual void keyPressed(unsigned char key, bool down);

	GameManager();
};