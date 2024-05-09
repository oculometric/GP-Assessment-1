#pragma once

#include "game_manager.h"

class MuseumGame : public GameManager
{
public:
	void start() override;
	void update(float delta_time) override;
	void mouseMove(int delta_x, int delta_y, bool down) override;
	void keyPressed(unsigned char key, bool down) override;
};