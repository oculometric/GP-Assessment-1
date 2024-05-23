#include "museum_game.h"

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "texture.h"
#include "object.h"
#include "material.h"
#include "scene_manager.h"
#include "matrix3.h"

using namespace std;

void MuseumGame::start(SceneManager* manager)
{
	// set up the scene the way we want it
	scene_manager = manager;

	scene_manager->addObject(scene_parent);
	scene_manager->setCamera(camera);

	player_light = scene_manager->getLight(1);
	*player_light = LightObject(LightType::POSITIONAL, Vector3{ 1.0f, 1.0f, 2.0f }, Vector3{ 0.5f, 0.5f, 0.5f }, Vector3{ 0.0f, 0.0f, -1.0f });
	player_light->attenuation_constant = 1.0f;
	player_light->attenuation_linear = 0.9f;
	player_light->spot_angle = 180.0f;
}

void MuseumGame::init()
{
	// create an empty object which will be the root of this scene
	scene_parent = new Object();

	// load textures
	Texture* concrete_tex = new Texture();
	Texture* wall_tex = new Texture();
	Texture* earth_tex = new Texture();
	Texture* checker_tex = new Texture();
	Texture* palms_tex = new Texture();
	Texture* pipes_tex = new Texture();
	concrete_tex->loadBMP("concrete_1k_t.bmp");
	wall_tex->loadBMP("wall_1k_t.bmp");
	earth_tex->loadBMP("earth_1k_t.bmp");
	checker_tex->loadBMP("checker_2k_t.bmp");
	palms_tex->loadBMP("palm_frond_variegated_t.bmp");
	pipes_tex->loadBMP("shiny_pipes_t.bmp");

	// load meshes
	Mesh* concrete_mesh = new Mesh("museum_concrete.obj");
	Mesh* wall_mesh = new Mesh("museum_white_walls.obj");
	Mesh* earth_mesh = new Mesh("museum_earth.obj");
	Mesh* checker_mesh = new Mesh("museum_checker.obj");
	Mesh* palms_mesh = new Mesh("museum_palms.obj");
	Mesh* pipes_mesh = new Mesh("museum_pipes.obj");

	// configure materials
	Material* concrete_mat = new Material(Vector3{ 1,1,1 }, 0.5f, concrete_tex);
	Material* wall_mat = new Material(Vector3{ 1,1,1 }, 0.5f, wall_tex);
	Material* earth_mat = new Material(Vector3{ 1,1,1 }, 0.5f, earth_tex);
	Material* checker_mat = new Material(Vector3{ 1,1,1 }, 0.5f, checker_tex);
	Material* palms_mat = new Material(Vector3{ 1,1,1 }, 0.5f, palms_tex);
	Material* pipes_mat = new Material(Vector3{ 1,1,1 }, 0.5f, pipes_tex);
	concrete_mesh->material = concrete_mat;
	wall_mesh->material = wall_mat;
	earth_mesh->material = earth_mat;
	checker_mesh->material = checker_mat;
	palms_mesh->material = palms_mat;
	pipes_mesh->material = pipes_mat;

	// configure mesh objects for the 4 parts of the museum
	concrete_obj = new MeshObject(concrete_mesh);
	wall_obj = new MeshObject(wall_mesh);
	earth_obj = new MeshObject(earth_mesh);
	checker_obj = new MeshObject(checker_mesh);
	palms_obj = new MeshObject(palms_mesh);
	pipes_obj= new MeshObject(pipes_mesh);

	// add all of these to the scene parent
	scene_parent->addChild(concrete_obj);
	scene_parent->addChild(wall_obj);
	scene_parent->addChild(earth_obj);
	scene_parent->addChild(checker_obj);
	scene_parent->addChild(palms_obj);
	scene_parent->addChild(pipes_obj);

	// set up our camera
	camera = new CameraObject(95.0f, 0.1f, 100.0f);
	camera->local_rotation = Vector3{ -90, 0, 0 };

	scene_parent->addChild(camera);
}

void MuseumGame::update(float delta_time)
{
	// calculate movement direction in local space (i.e. -z is forwards, etc)
	Vector3 local_direction = Vector3
	{
		(float)(((control_key_states >> 3) & 1) - ((control_key_states >> 2) & 1)),
		(float)(((control_key_states >> 5) & 1) - ((control_key_states >> 4) & 1)),
		(float)(((control_key_states >> 1) & 1) - ((control_key_states >> 0) & 1))
	};
	local_direction = norm(local_direction) * 2.0f;

	// construct rotation matrices from camera euler
	Vector3 euler = scene_manager->getCamera()->local_rotation * ((float)M_PI / 180.0f);
	Matrix3 rot_x =
	{
		1, 0, 0,
		0, cos(-euler.x), -sin(-euler.x),
		0, sin(-euler.x), cos(-euler.x)
	};
	Matrix3 rot_y =
	{
		cos(-euler.y), 0, sin(-euler.y),
		0, -1, 0,
		-sin(-euler.y), 0, cos(-euler.y)
	};
	Matrix3 rot_z =
	{
		cos(-euler.z), -sin(-euler.z), 0,
		sin(-euler.z),  cos(-euler.z), 0,
		0, 0, 1
	};
	// apply control direction as velocity
	camera->velocity_lin = ((rot_z * rot_x * rot_y) * local_direction);
	player_light->local_position = camera->local_position;
}

void MuseumGame::mouseMove(int delta_x, int delta_y, bool down)
{
	if (!down) return;
	scene_manager->getCamera()->local_rotation.z -= (float)delta_x * 0.2f;
	scene_manager->getCamera()->local_rotation.x = min(max(scene_manager->getCamera()->local_rotation.x - ((float)delta_y * 0.2f), -170.0f), -10.0f);
}

void MuseumGame::keyPressed(unsigned char key, bool down)
{
	// byte-pack the key states for movement
	if (tolower(key) == 'w')
	{
		if (down)
			control_key_states |=  0b00000001;
		else
			control_key_states &= ~0b00000001;
	}
	else if (tolower(key) == 's')
	{
		if (down)
			control_key_states |=  0b00000010;
		else
			control_key_states &= ~0b00000010;
	}
	else if (tolower(key) == 'a')
	{
		if (down)
			control_key_states |=  0b00000100;
		else
			control_key_states &= ~0b00000100;
	}
	else if (tolower(key) == 'd')
	{
		if (down)
			control_key_states |=  0b00001000;
		else
			control_key_states &= ~0b00001000;
	}
	else if (tolower(key) == 'e')
	{
		if (down)
			control_key_states |=  0b00010000;
		else
			control_key_states &= ~0b00010000;
	}
	else if (tolower(key) == 'q')
	{
		if (down)
			control_key_states |=  0b00100000;
		else
			control_key_states &= ~0b00100000;
	}
}

void MuseumGame::destroy()
{
	// remove our scene parent from the actual scene
	scene_parent->removeFromParent();
	// clear the scene camera
	scene_manager->setCamera(nullptr);
}
