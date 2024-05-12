#pragma once

#include "game_manager.h"

class MuseumGame : public GameManager
{
private:
	// stores key states in the format 0b00qedasw
	unsigned char control_key_states = 0;
public:
	void start() override;
	void update(float delta_time) override;
	void mouseMove(int delta_x, int delta_y, bool down) override;
	void keyPressed(unsigned char key, bool down) override;
};