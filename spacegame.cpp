#include "spacegame.h"

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "glut_callback_handlers.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

void spacegame::move_and_bounce_vertex(vector2& v, vector2& v_m, float f)
{
	vector2 projected_point = v + (v_m * f);
	if (projected_point.x > 1.0f)
		v_m.x = -abs(v_m.x);
	if (projected_point.x < -1.0f)
		v_m.x = abs(v_m.x);
	if (projected_point.y > 1.0f)
		v_m.y = -abs(v_m.y);
	if (projected_point.y < -1.0f)
		v_m.y = abs(v_m.y);
	v = projected_point;
}

inline float randf()
{
	return (((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f;
}

spacegame::spacegame(int argc, char* argv[], unsigned int x, unsigned int y)
{
	glut_callback_handlers::init(this);

	last_frame_time = std::chrono::high_resolution_clock::now();

	srand(last_frame_time.time_since_epoch().count());

	v1_m = norm(vector2{ randf(), randf() });
	v2_m = norm(vector2{ randf(), randf() });
	v3_m = norm(vector2{ randf(), randf() });

	m = new mesh("teapot.obj");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(x, y);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("GP");
	glutDisplayFunc(glut_callback_handlers::display);
	glutMotionFunc(glut_callback_handlers::mouse_move);
	glutPassiveMotionFunc(glut_callback_handlers::mouse_move_passive);
	glutMouseFunc(glut_callback_handlers::mouse_click);
	glutKeyboardFunc(glut_callback_handlers::key_down);
	glutKeyboardUpFunc(glut_callback_handlers::key_up);
	glutIgnoreKeyRepeat(1);
	glutTimerFunc(1000 / 60, glut_callback_handlers::frame_refresh, 1000 / 60);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glFrontFace(GL_CCW);

	camera_position.z = 2.0f;
	camera_rotation = vector3{ 0, 0, 0 };

	glutMainLoop();
}

void spacegame::display()
{
	
	auto time_now = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(time_now - last_frame_time).count() / 1000000000.0f;
	last_frame_time = time_now;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO: transform camera_local_velocity by the camera_rotation
	camera_position += camera_local_velocity * 0.5 * delta_time;
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(camera_rotation.y, 0, 1, 0);
	glRotatef(camera_rotation.x, 1, 0, 0);
	glRotatef(camera_rotation.z, 0, 0, 1);
	glTranslatef(-camera_position.x, -camera_position.x, -camera_position.z);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 0.1, 10);

	draw_mesh();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.8, 0.8, -0.8);
	glRotatef(camera_rotation.y, 0, 1, 0);
	glRotatef(camera_rotation.x, 1, 0, 0);
	glRotatef(camera_rotation.z, 0, 0, 1);

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

	glFlush();

}

void spacegame::mouse_move(int x, int y)
{
	int diff_x = x - last_mouse_x;
	int diff_y = y - last_mouse_y;	

	camera_rotation.x += diff_y * 0.5f;
	camera_rotation.z += diff_x * 0.5f;

	last_mouse_x = x;
	last_mouse_y = y;
}

void spacegame::mouse_move_passive(int x, int y)
{
	last_mouse_x = x;
	last_mouse_y = y;
}

void spacegame::mouse_click(int button, int state, int x, int y)
{
	std::cout << "button " << button << (state ? " up" : " down") << std::endl;
}

void spacegame::key_down(uint8_t key, int x, int y)
{
	if (key == 'w') camera_local_velocity.z += -1.0f;
	if (key == 's') camera_local_velocity.z += 1.0f;
	if (key == 'a') camera_local_velocity.x += -1.0f;
	if (key == 'd') camera_local_velocity.x += 1.0f;
}

void spacegame::key_up(uint8_t key, int x, int y)
{
	if (key == 'w') camera_local_velocity.z -= -1.0f;
	if (key == 's') camera_local_velocity.z -= 1.0f;
	if (key == 'a') camera_local_velocity.x -= -1.0f;
	if (key == 'd') camera_local_velocity.x -= 1.0f;
}

void spacegame::frame_refresh(int value)
{
	glutPostRedisplay();
	glutTimerFunc(value, glut_callback_handlers::frame_refresh, value);
}

void spacegame::draw_mesh()
{
	if (m->triangles == NULL || m->vertices == NULL) return;

	glPolygonMode(GL_BACK, GL_LINE);

	glBegin(GL_TRIANGLES);
	for (uint32_t i = 0; i < m->tris_count(); i++)
	{
		vector3 vert = m->vertices[m->triangles[i]] * 0.5f;
		glColor3f(vert.x + 0.5f, vert.y + 0.5f, vert.z + 0.5f);
		glVertex3f(vert.x * 0.5f, vert.y * 0.5f, vert.z * 0.5f);
	}
	glEnd();
}


spacegame::~spacegame()
{ }
