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
	scene_manager = manager;

	scene_manager->addObject(scene_parent);
	scene_manager->setCamera(camera);

	LightObject* light_1 = scene_manager->getLight(1);
	*light_1 = LightObject(LightType::POSITIONAL, Vector3{ 1.0f, 1.0f, 1.0f });
}

void MuseumGame::init()
{
	scene_parent = new Object();

	Texture* concrete_tex = new Texture();
	Texture* wall_tex = new Texture();
	Texture* earth_tex = new Texture();
	Texture* checker_tex = new Texture();
	concrete_tex->loadBMP("concrete_1k_t.bmp");
	wall_tex->loadBMP("wall_1k_t.bmp");
	earth_tex->loadBMP("earth_1k_t.bmp");
	checker_tex->loadBMP("checker_2k_t.bmp");

	Mesh* concrete_mesh = new Mesh("museum_concrete.obj");
	Mesh* wall_mesh = new Mesh("museum_white_walls.obj");
	Mesh* earth_mesh = new Mesh("museum_earth.obj");
	Mesh* checker_mesh = new Mesh("museum_checker.obj");

	Material* concrete_mat = new Material(Vector3{ 1,1,1 }, 0.5f, concrete_tex);
	Material* wall_mat = new Material(Vector3{ 1,1,1 }, 0.5f, wall_tex);
	Material* earth_mat = new Material(Vector3{ 1,1,1 }, 0.5f, earth_tex);
	Material* checker_mat = new Material(Vector3{ 1,1,1 }, 0.5f, checker_tex);
	concrete_mesh->material = concrete_mat;
	wall_mesh->material = wall_mat;
	earth_mesh->material = earth_mat;
	checker_mesh->material = checker_mat;

	concrete_obj = new MeshObject(concrete_mesh);
	wall_obj = new MeshObject(wall_mesh);
	earth_obj = new MeshObject(earth_mesh);
	checker_obj = new MeshObject(checker_mesh);

	scene_parent->addChild(concrete_obj);
	scene_parent->addChild(wall_obj);
	scene_parent->addChild(earth_obj);
	scene_parent->addChild(checker_obj);

	camera = new CameraObject(95.0f, 0.1f, 100.0f);
	camera->local_rotation = Vector3{ -90, 0, 0 };

	scene_parent->addChild(camera);
}

void MuseumGame::update(float delta_time)
{
	Vector3 local_direction = Vector3
	{
		(float)(((control_key_states >> 3) & 1) - ((control_key_states >> 2) & 1)),
		(float)(((control_key_states >> 5) & 1) - ((control_key_states >> 4) & 1)),
		(float)(((control_key_states >> 1) & 1) - ((control_key_states >> 0) & 1))
	};
	local_direction = norm(local_direction) * 2.0f;

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
	scene_manager->getCamera()->velocity_lin = ((rot_z * rot_x * rot_y) * local_direction);
}

void MuseumGame::mouseMove(int delta_x, int delta_y, bool down)
{
	if (!down) return;
	scene_manager->getCamera()->local_rotation.z -= (float)delta_x * 0.2f;
	scene_manager->getCamera()->local_rotation.x = min(max(scene_manager->getCamera()->local_rotation.x - ((float)delta_y * 0.2f), -170.0f), -10.0f);
}

void MuseumGame::keyPressed(unsigned char key, bool down)
{
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
	scene_parent->removeFromParent();
	scene_manager->setCamera(nullptr);
}
