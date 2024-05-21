#pragma once

#include "game_manager.h"
#include "object.h"

class MuseumGame : public GameManager
{
private:
	// stores key states in the format 0b00qedasw
	unsigned char control_key_states = 0;

	// pointers for allocated objects

	LightObject* player_light = nullptr;
	Object* scene_parent = nullptr;

	MeshObject* concrete_obj = nullptr;
	MeshObject* wall_obj = nullptr;
	MeshObject* earth_obj = nullptr;
	MeshObject* checker_obj = nullptr;

	CameraObject* camera = nullptr;
public:
	void init() override;
	void start(SceneManager* manager) override;
	void update(float delta_time) override;
	void mouseMove(int delta_x, int delta_y, bool down) override;
	void keyPressed(unsigned char key, bool down) override;
	void destroy() override;
};