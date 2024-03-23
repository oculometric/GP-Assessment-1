#include "spacegame.h"

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "glut_callback_handlers.h"
#include "matrix3.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

inline float randf()
{
	return (((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f;
}

SpaceGame::SpaceGame(int argc, char* argv[], unsigned int x, unsigned int y)
{
	glut_callback_handlers::init(this);

	last_frame_time = std::chrono::high_resolution_clock::now();

	srand(last_frame_time.time_since_epoch().count());

	root_object = new Object(ObjectType::EMPTY);
	root_object->name = "root";
	active_camera = new Object(ObjectType::CAMERA, Vector3{1,0,2}, Vector3{0,23.0f,0});
	root_object->addChild(active_camera, true);

	Object* teapot = new Object(ObjectType::MESH);
	teapot->name = "teapot";
	teapot->geometry = new Mesh("teapot.obj");
	root_object->addChild(teapot, true);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(x, y);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("GP");
	glutDisplayFunc(glut_callback_handlers::display);
	glutMotionFunc(glut_callback_handlers::mouseMove);
	glutPassiveMotionFunc(glut_callback_handlers::mouseMovePassive);
	glutMouseFunc(glut_callback_handlers::mouseClick);
	glutKeyboardFunc(glut_callback_handlers::keyDown);
	glutKeyboardUpFunc(glut_callback_handlers::keyUp);
	glutIgnoreKeyRepeat(1);
	glutTimerFunc(1000 / 60, glut_callback_handlers::frameRefresh, 1000 / 60);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glFrontFace(GL_CCW);

	glutMainLoop();
}

void SpaceGame::display()
{	
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (active_camera)
		renderFromCamera(active_camera);

	glFlush();

}

void SpaceGame::mouseMove(int x, int y)
{
	if (!active_camera) return;

	int diff_x = x - last_mouse_x;
	int diff_y = y - last_mouse_y;

	active_camera->local_rotation.x += diff_y * 0.5f;
	active_camera->local_rotation.z += diff_x * 0.5f;

	last_mouse_x = x;
	last_mouse_y = y;
}

void SpaceGame::mouseMovePassive(int x, int y)
{
	last_mouse_x = x;
	last_mouse_y = y;
}

void SpaceGame::mouseClick(int button, int state, int x, int y)
{
	std::cout << "button " << button << (state ? " up" : " down") << std::endl;
}

void SpaceGame::keyDown(uint8_t key, int x, int y)
{
	if (key == 'w') camera_local_velocity.z += -1.0f;
	if (key == 's') camera_local_velocity.z += 1.0f;
	if (key == 'a') camera_local_velocity.x += -1.0f;
	if (key == 'd') camera_local_velocity.x += 1.0f;
	if (key == 'q') camera_local_velocity.y += -1.0f;
	if (key == 'e') camera_local_velocity.y += 1.0f;
}

void SpaceGame::keyUp(uint8_t key, int x, int y)
{
	if (key == 'w') camera_local_velocity.z -= -1.0f;
	if (key == 's') camera_local_velocity.z -= 1.0f;
	if (key == 'a') camera_local_velocity.x -= -1.0f;
	if (key == 'd') camera_local_velocity.x -= 1.0f;
	if (key == 'q') camera_local_velocity.y -= -1.0f;
	if (key == 'e') camera_local_velocity.y -= 1.0f;
}

void SpaceGame::frameRefresh(int value)
{
	// get delta time
	auto time_now = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(time_now - last_frame_time).count() / 1000000000.0f;
	last_frame_time = time_now;

	if (active_camera)
	{
		Vector3 angle = (active_camera->local_rotation * M_PI) / 180.0f;
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
		active_camera->local_position += camera_global_velocity * Vector3{ 1,-1,1 } *0.5 * delta_time;
	}

	glutPostRedisplay();
	glutTimerFunc(value, glut_callback_handlers::frameRefresh, value);
}

void SpaceGame::renderFromCamera(Object* camera)
{
	if (!camera) return;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(camera->local_rotation.y, 0.0f, 1.0f, 0.0f);
	glRotatef(camera->local_rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(camera->local_rotation.z, 0.0f, 0.0f, 1.0f);
	glTranslatef(-camera->local_position.x, -camera->local_position.y, -camera->local_position.z);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 0.1, 10);

	if (root_object)
	{
		renderHierarchy(root_object);
	}

	renderAxesGizmo(camera);
}

void SpaceGame::renderHierarchy(Object* root)
{
	if (!root) return;

	// push new matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// apply transformation matrix of root
	glScalef(root->local_scale.x, root->local_scale.y, root->local_scale.z);
	glRotatef(root->local_rotation.y, 0.0f, 1.0f, 0.0f);
	glRotatef(root->local_rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(root->local_rotation.z, 0.0f, 0.0f, 1.0f);
	glTranslatef(root->local_position.x, root->local_position.y, root->local_position.z);
	
	// draw root
	drawObject(root);

	// iterate over children, calling renderHierarchy on each
	for (Object* child : root->children)
	{
		renderHierarchy(child);
	}
	
	// pop matrix
	glPopMatrix();
}

void SpaceGame::renderAxesGizmo(Object* camera)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.8f, 0.8f, -0.8);
	glRotatef(camera->local_rotation.y, 0.0f, 1.0f, 0.0f);
	glRotatef(camera->local_rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(camera->local_rotation.z, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0.2, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0.2, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 0.2);
	}
	glEnd();
}

void SpaceGame::drawObject(Object* obj)
{
	std::cout << "drawing object " << obj->name << std::endl;
	if (obj->object_type != ObjectType::MESH) return;
	if (!obj->geometry) return;
	if (obj->geometry->triangles == NULL || obj->geometry->vertices == NULL) return;

	glPolygonMode(GL_BACK, GL_LINE);

	glBegin(GL_TRIANGLES);
	for (uint32_t i = 0; i < obj->geometry->trisCount(); i++)
	{
		Vector3 vert = obj->geometry->vertices[obj->geometry->triangles[i]] * 0.5f;
		glColor3f(vert.x + 0.5f, vert.y + 0.5f, vert.z + 0.5f);
		glVertex3f(vert.x * 0.5f, vert.y * 0.5f, vert.z * 0.5f);
	}
	glEnd();
}


SpaceGame::~SpaceGame()
{ }
