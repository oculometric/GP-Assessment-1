#pragma once

#include "game_manager.h"
#include "vector3.h"
#include "object.h"

#include <vector>

void switchRealities();

class SpaceGame : public GameManager
{
private:
	Vector3 camera_local_velocity;
	MeshObject* ship;
	Object* camera_focus;

	Vector3* asteroid_points;
	size_t num_asteroid_points;
	std::vector<MeshObject*> loaded_asteroids;
	Mesh* asteroid_mesh;

	TextObject* position_text;
	TextObject* velocity_text;
	TextObject* ship_rot_text;
	TextObject* cam_rot_text;

	Material* particle_mat;

	MeshObject* planet;
	MeshObject* moon;
	MeshObject* overlays[4];

	float acceleration = 0.0f;

public:
	void start() override;
	void update(float delta_time) override;
	void mouseMove(int delta_x, int delta_y, bool down) override;
	void keyPressed(unsigned char key, bool down) override;
	void destroy();
};