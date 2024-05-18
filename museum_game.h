#pragma once

#include "game_manager.h"
#include "object.h"

class MuseumGame : public GameManager
{
private:
	// stores key states in the format 0b00qedasw
	unsigned char control_key_states = 0;
	LightObject* player_light = nullptr;
	Object* scene_parent;
public:
	void init() override;
	void start(SceneManager* manager) override;
	void update(float delta_time) override;
	void mouseMove(int delta_x, int delta_y, bool down) override;
	void keyPressed(unsigned char key, bool down) override;
};