#include "spacegame.h"

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "glut_callback_handlers.h"
#include "matrix3.h"
#include <queue>

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

	srand((unsigned int)last_frame_time.time_since_epoch().count());

	// core scene initialisation
	root_object = new Object();
	root_object->name = "root";
	active_camera = new CameraObject(90.0f, 0.01f, 64.0f, (float)x/(float)y, Vector3{ 0,-1,1 }, Vector3{ -23.0f,0,0 });

	// now adding some demo objects
	Object* teapot = new MeshObject(new Mesh("teapot.obj"));
	teapot->name = "teapot";
	teapot->velocity_ang.z = 30.0f;
	root_object->addChild(teapot, true);

	Object* teapot2 = new MeshObject(new Mesh("teapot.obj"), Vector3{ 2,0,0 });
	teapot2->name = "teapot";
	root_object->addChild(teapot2, true);

	Object* suzanne = new MeshObject(new Mesh("suzanne.obj"), Vector3{ 0,1,2 }, Vector3{ 0,0,0 }, Vector3{ 3,3,3 });
	suzanne->name = "suzanne";
	suzanne->velocity_lin.z = 0.1f;
	teapot->addChild(suzanne, true);

	ship = new MeshObject(new Mesh("beholder_v3.obj"), Vector3{ 0,2,0 }, Vector3{ 0,-90,90 }, Vector3{ 0.5f,0.5f,0.5f });
	ship->velocity_ang.x = -60.0f;
	ship->name = "ship";
	suzanne->addChild(ship, true);

	ship->addChild(active_camera, true);

	// GLUT and GL initialisation
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(x, y);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("GP");
	// configure callbacks
	glutDisplayFunc(glut_callback_handlers::display);
	glutMotionFunc(glut_callback_handlers::mouseMove);
	glutPassiveMotionFunc(glut_callback_handlers::mouseMovePassive);
	glutMouseFunc(glut_callback_handlers::mouseClick);
	glutKeyboardFunc(glut_callback_handlers::keyDown);
	glutKeyboardUpFunc(glut_callback_handlers::keyUp);
	glutIgnoreKeyRepeat(1);
	glutReshapeFunc(glut_callback_handlers::resizeWindow);
	glutTimerFunc(1000 / 60, glut_callback_handlers::frameRefresh, 1000 / 60);
	// enable depth buffer/testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// cull backfaces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// counter-clockwise winding
	glFrontFace(GL_CCW);
	// enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	// aaaaaand, go!
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

	// handle camera movement
	if (active_camera)
	{
		Vector3 angle = active_camera->local_rotation * (M_PI / 180.0f);
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
		active_camera->local_position += camera_global_velocity * Vector3{ 1,-1,1 } * delta_time;
	}

	// perform physics update for all objects
	std::queue<Object*> physics_tick_queue;
	physics_tick_queue.push(root_object);
	while (!physics_tick_queue.empty())
	{
		Object* obj = physics_tick_queue.front();
		physics_tick_queue.pop();

		obj->performPhysicsUpdate(delta_time);

		for (Object* child_obj : obj->children) physics_tick_queue.push(child_obj);
	}

	// check for errors
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "ERROR DETECTED:" << std::endl << "    ";
		switch (err)
		{
		case GL_INVALID_ENUM: std::cout << "invalid enum"; break;
		case GL_INVALID_VALUE: std::cout << "invalid value"; break;
		case GL_INVALID_OPERATION: std::cout << "invalid operation"; break;
		case GL_STACK_OVERFLOW: std::cout << "stack overflow"; break;
		case GL_STACK_UNDERFLOW: std::cout << "stack underflow"; break;
		case GL_OUT_OF_MEMORY: std::cout << "out of memory"; break;
		}
		std::cout << std::endl;
	}

	// trigger drawing routine
	glutPostRedisplay();
	glutTimerFunc(value, glut_callback_handlers::frameRefresh, value);
}

void SpaceGame::resizeWindow(int x, int y)
{
	glViewport(0, 0, x, y);
	active_camera->aspect_ratio = (float)x / (float)y;
}

void SpaceGame::renderFromCamera(CameraObject* camera)
{
	// if the camera supplied is null, return
	if (!camera) return;

	// compute a matrix for the camera's transform (i.e. world-to-view)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Object* camera_matrix_stack = camera;
	while (camera_matrix_stack != NULL)
	{
		glScalef(1.0f / camera_matrix_stack->local_scale.x, 1.0f / camera_matrix_stack->local_scale.y, 1.0f / camera_matrix_stack->local_scale.z);
		glRotatef(camera_matrix_stack->local_rotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.z, 0.0f, 0.0f, 1.0f);
		glTranslatef(-camera_matrix_stack->local_position.x, -camera_matrix_stack->local_position.y, -camera_matrix_stack->local_position.z);
		
		camera_matrix_stack = camera_matrix_stack->parent;
	}

	// set the projection matrix to be a simple perspective matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->fov_degrees, camera->aspect_ratio, camera->near_clip, camera->far_clip);

	float light_pos[4] = { 0,0,1,0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	// render the entire object heirarchy
	if (root_object)
		renderHierarchy(root_object);

	// render the gizmo
	renderAxesGizmo(camera);
}

void SpaceGame::renderHierarchy(Object* root)
{
	if (!root) return;

	// push the matrix down
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// apply transformation matrix of root
	glTranslatef(root->local_position.x, root->local_position.y, root->local_position.z);
	glRotatef(-root->local_rotation.z, 0.0f, 0.0f, 1.0f);
	glRotatef(-root->local_rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(-root->local_rotation.y, 0.0f, 1.0f, 0.0f);
	glScalef(root->local_scale.x, root->local_scale.y, root->local_scale.z);

	// draw root
	if (root->getType() == ObjectType::MESH)
		drawObject((MeshObject*)root);

	// iterate over children, calling renderHierarchy on each
	for (Object* child : root->children)
	{
		renderHierarchy(child);
	}
	
	// pop matrix
	glPopMatrix();
}

void SpaceGame::renderAxesGizmo(CameraObject* camera)
{
	// reset the world-to-view camera stack, moving the axes to the top left of the screen
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-camera->aspect_ratio + 0.2f, 0.8f, -0.8f);
	Object* camera_matrix_stack = camera;
	while (camera_matrix_stack != NULL)
	{
		glRotatef(camera_matrix_stack->local_rotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(camera_matrix_stack->local_rotation.z, 0.0f, 0.0f, 1.0f);

		camera_matrix_stack = camera_matrix_stack->parent;
	}

	// orthographic projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-camera->aspect_ratio, camera->aspect_ratio, -1, 1, -1, 1);

	// disable lighting
	glDisable(GL_LIGHTING);

	// draw three lines on the three axes (coloured accordingly)
	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0.18f, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0.18f, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 0.18f);
	}
	glEnd();

	// reenable lighting
	glEnable(GL_LIGHTING);
}

void SpaceGame::drawObject(MeshObject* obj)
{
	// if the object isn't a mesh, or it's empty, or it has no triangles, give up
	if (!obj->geometry) return;
	if (obj->geometry->triangles == NULL || obj->geometry->vertices == NULL) return;

	// fill from front, draw as lines from back
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	float col[4] = { 0,0,0,0 };
	col[0] = (float)obj->name[0] / 255.0f;
	col[1] = (float)obj->name[1] / 255.0f;
	col[2] = (float)obj->name[2] / 255.0f;

	// draw triangle mesh
	glBegin(GL_TRIANGLES);
	for (uint32_t i = 0; i < obj->geometry->trisCount(); i++)
	{
		Vector3 vert = obj->geometry->vertices[obj->geometry->triangles[i]] * 0.5f;
		Vector3 normal = obj->geometry->vertex_normals[i];
		normal = norm(normal);
		Vector2 uv = obj->geometry->uvs[i];
		float col[4] = { normal.x, normal.y, normal.z, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, col);

		glTexCoord2f(uv.x, uv.y);
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(vert.x * 0.5f, vert.y * 0.5f, vert.z * 0.5f);
	}
	glEnd();
}


SpaceGame::~SpaceGame()
{ }
