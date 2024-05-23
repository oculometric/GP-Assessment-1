#pragma once

#include "game_manager.h"
#include "vector3.h"
#include "object.h"

#include <vector>

class SpaceGame : public GameManager
{
private:
	// most of these are self-explanatory

	Vector3 camera_local_velocity;
	MeshObject* ship = nullptr;
	Object* camera_focus = nullptr;
	CameraObject* camera = nullptr;
	Object* scene_parent = nullptr;

	// mesh objects placed into the overlay
	
	MeshObject* overlay_ship = nullptr;
	MeshObject* spinning_ico_0 = nullptr;
	MeshObject* spinning_ico_1 = nullptr;
	MeshObject* spinning_ico_2 = nullptr;

	// info for asteroid field
	
	Vector3* asteroid_points = nullptr;
	size_t num_asteroid_points;
	std::vector<MeshObject*> loaded_asteroids;
	Mesh* asteroid_mesh = nullptr;

	// text objects placed into the overlay
	
	TextObject* position_text = nullptr;
	TextObject* velocity_text = nullptr;
	TextObject* ship_rot_text = nullptr;
	TextObject* cam_rot_text = nullptr;

	// pointers to allocated objects

	Material* particle_mat = nullptr;

	MeshObject* planet = nullptr;
	MeshObject* moon = nullptr;

	Texture* skybox_texture = nullptr;

	// spaceship acceleration

	float acceleration = 0.0f;

public:
	void init() override;
	void start(SceneManager* manager) override;
	void update(float delta_time) override;
	void mouseMove(int delta_x, int delta_y, bool down) override;
	void keyPressed(unsigned char key, bool down) override;
	void destroy();
};