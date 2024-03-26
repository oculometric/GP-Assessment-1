#pragma once

#include "game_manager.h"
#include "vector3.h"
#include "object.h"

class SpaceGame : public GameManager
{
private:
	Vector3 camera_local_velocity;
	MeshObject* ship;
	Object* camera_focus;

public:
	void start() override;
	void update(float delta_time) override;
	void mouseMove(int delta_x, int delta_y, bool down) override;
	void keyPressed(unsigned char key, bool down) override;
};