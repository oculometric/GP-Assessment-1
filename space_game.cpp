#include "space_game.h"
#include "scene_manager.h"
#include "object.h"
#include "matrix3.h"
#include "GL/freeglut.h"
#include "GL/glut.h"

#define _USE_MATH_DEFINES
#include <math.h>

void SpaceGame::start()
{
	ship = new MeshObject(new Mesh("beholder_v3.obj"));
	ship->name = "spaceship";
	scene_manager->addObject(ship);

	camera_focus = new Object();
	ship->addChild(camera_focus, true);

	CameraObject* cam = scene_manager->getCamera();
	cam->removeFromParent(true);

	cam->local_position = Vector3{ 0,0,3 };
	cam->local_rotation = Vector3{ 180.0f,180.0f,180.0f };
	camera_focus->addChild(cam, true);

	ShowCursor(false);
}

void SpaceGame::update(float delta_time)
{
	// handle camera movement
	Vector3 angle = scene_manager->getCamera()->local_rotation * (M_PI / 180.0f);
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
	scene_manager->getCamera()->local_position += camera_global_velocity * Vector3{1,-1,1} *delta_time;
}

void SpaceGame::mouseMove(int delta_x, int delta_y, bool down)
{
	if (!down) return;
	camera_focus->local_rotation.x += delta_y * 0.5f;
	camera_focus->local_rotation.z += delta_x * 0.5f;
}

void SpaceGame::keyPressed(unsigned char key, bool down)
{
	float up_down = down ? 1.0f : -1.0f;
	if (key == 'w') camera_local_velocity.z += -1.0f * up_down;
	if (key == 's') camera_local_velocity.z += 1.0f * up_down;
	/*if (key == 'a') camera_local_velocity.x += -1.0f * up_down;
	if (key == 'd') camera_local_velocity.x += 1.0f * up_down;
	if (key == 'q') camera_local_velocity.y += -1.0f * up_down;
	if (key == 'e') camera_local_velocity.y += 1.0f * up_down;*/
}
