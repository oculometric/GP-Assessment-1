#include "space_game.h"
#include "scene_manager.h"
#include "object.h"
#include "matrix3.h"
#include "GL/freeglut.h"
#include "GL/glut.h"
#include "point_generator.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define ASTEROID_DENSITY 0.00003f
#define ASTEROID_MAP_SIZE 300
#define ASTEROID_MAP_RADIUS ASTEROID_MAP_SIZE / 2
#define NUM_ASTEROIDS (size_t)(ASTEROID_MAP_SIZE * ASTEROID_MAP_SIZE * ASTEROID_MAP_SIZE * ASTEROID_DENSITY)

float randf()
{
	return (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
}

void SpaceGame::start()
{
	asteroid_points = new Vector3[NUM_ASTEROIDS];
	generatePoints(NUM_ASTEROIDS, 2, Vector3{ -ASTEROID_MAP_RADIUS, -ASTEROID_MAP_RADIUS, -ASTEROID_MAP_RADIUS }, Vector3{ ASTEROID_MAP_RADIUS,ASTEROID_MAP_RADIUS,ASTEROID_MAP_RADIUS }, asteroid_points);

	ship = new MeshObject(new Mesh("beholder_v4.obj"));
	ship->name = "spaceship";
	ship->geometry->material = new Material(Vector3{ 1.0f, 0.0f, 1.0f }, 0.7f, new Texture());
	ship->geometry->material->albedo->loadBMP("beholder_v4_t.bmp");
	scene_manager->addObject(ship);

	camera_focus = new Object();
	camera_focus->local_position = Vector3{ 0.0f, -4.4f, 2.6f };
	camera_focus->local_rotation.x = 180.0f;
	ship->addChild(camera_focus, true);

	CameraObject* cam = scene_manager->getCamera();
	cam->removeFromParent(true);

	cam->local_position = Vector3{ 0,0,12 };
	cam->local_rotation = Vector3{ 180.0f,180.0f,180.0f };
	cam->far_clip = 3000.0f;
	camera_focus->addChild(cam, true);

	MeshObject* planet = new MeshObject(new Mesh("planet.obj"));
	planet->geometry->material = new Material(Vector3{ 1,0,1 }, 0.1f, new Texture());
	planet->geometry->material->albedo->loadBMP("moon_and_planet_t.bmp");
	planet->local_position = Vector3{ 1200, 200, 400 };
	planet->velocity_ang.z = 2.0f;
	planet->local_scale = Vector3{ 100, 100, 100 };
	scene_manager->addObject(planet);

	MeshObject* moon = new MeshObject(new Mesh("moon.obj"));
	moon->geometry->material = planet->geometry->material;
	moon->local_position = Vector3{ 15, 0, 0 };
	moon->velocity_ang.z = 0.2f;
	moon->local_scale = Vector3{ 1, 1, 1 };
	planet->addChild(moon, true);

	// enable fog
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 3.0f);
	glFogf(GL_FOG_END, 2000.0f);
	float fog_colour[4] = { 0.05f, 0.05f, 0.08f, 1.0f };
	glFogfv(GL_FOG_COLOR, fog_colour);
	glFogf(GL_FOG_DENSITY, 0.4f);

	LightObject* star = scene_manager->getLight(0);
	*star = LightObject(LightType::DIRECTIONAL, Vector3{ 3.0f, 2.8f, 2.6f });
	star->direction = Vector3{ 0,0.2,-1.0f };
	star->ambient_colour = Vector3{ 0.05f, 0.05f, 0.2f };

	scene_manager->skybox = new Texture();
	scene_manager->skybox->loadBMP("nasa_goddard_gaia_dr2_deep_star_map.bmp");
	
	asteroid_mesh = new Mesh("asteroid_0.obj");

	for (int a = 0; a < NUM_ASTEROIDS; a++)
	{
		MeshObject* new_asteroid = new MeshObject(asteroid_mesh, asteroid_points[a]);
		loaded_asteroids.push_back(new_asteroid);
		scene_manager->addObject(new_asteroid);
		new_asteroid->velocity_ang = Vector3{ randf() * 20.0f, randf() * 20.0f, randf() * 20.0f };
		float scale_xy = (randf() * 0.5f) + 1.25f;
		float scale_xz = (randf() * 0.6f) + 1.2f;
		float scale_x = (randf() + 1.2f) * 2.6f;
		new_asteroid->local_scale = Vector3{ scale_x, scale_x * scale_xy, scale_x * scale_xz };
		new_asteroid->velocity_lin = Vector3{ randf() * 0.2f, randf() * 0.2f, randf() * 0.2f };
	}
}

void SpaceGame::update(float delta_time)
{
	// handle camera movement
	Vector3 angle = scene_manager->getCamera()->local_rotation * ((float)M_PI / 180.0f);
	Matrix3 rot_x = { 1,  0,             0,
						0,  cos(angle.x), -sin(angle.x),
						0,  sin(angle.x),  cos(angle.x) };

	Matrix3 rot_y = { cos(angle.y),  0,  sin(angle.y),
						0,             -1,  0,
						-sin(angle.y),  0,  cos(angle.y) };

	Matrix3 rot_z = { cos(angle.z), -sin(angle.z),  0,
						sin(angle.z),  cos(angle.z),  0,
						0,             0,             1 };

	// apply y, x, z rotations
	Vector3 camera_global_velocity = (rot_z * rot_x * rot_y) * camera_local_velocity;
	scene_manager->getCamera()->local_position += camera_global_velocity * Vector3{1,-1,1} * delta_time;

	// handle acceleration, in ship direction
	angle = ship->local_rotation * ((float)M_PI / 180.0f);
	rot_x =
	{
		1, 0, 0,
		0, cos(-angle.x), -sin(-angle.x),
		0, sin(-angle.x), cos(-angle.x)
	};
	rot_y =
	{
		cos(-angle.y), 0, sin(-angle.y),
		0, -1, 0,
		-sin(-angle.y), 0, cos(-angle.y)
	};
	rot_z =
	{
		cos(-angle.z), -sin(-angle.z), 0,
		sin(-angle.z),  cos(-angle.z), 0,
		0, 0, 1
	};
	ship->velocity_lin += (((rot_z * rot_x * rot_y)) * Vector3 { 0, 0, acceleration }) * delta_time;
	acceleration *= powf(0.9f, delta_time);
}

void SpaceGame::mouseMove(int delta_x, int delta_y, bool down)
{
	if (!down) return;
	camera_focus->local_rotation.x += delta_y * 0.5f;
	camera_focus->local_rotation.z += delta_x * 0.5f;
}

void SpaceGame::keyPressed(unsigned char key, bool down)
{
	float sensitivity = 8.0f;
	float up_down = down ? sensitivity : -sensitivity;
	if (key == 'e' || key == 'E') acceleration += up_down;
	if (key == 'q' || key == 'Q') acceleration -= up_down;
	if (key == 'w' || key == 'W') ship->velocity_ang.x += up_down * 10.0f;
	if (key == 's' || key == 'S') ship->velocity_ang.x -= up_down * 10.0f;
	if (key == 'a' || key == 'A') ship->velocity_ang.z += up_down * 10.0f;
	if (key == 'd' || key == 'D') ship->velocity_ang.z -= up_down * 10.0f;
}
