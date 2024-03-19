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

	/*m = new mesh(8, 12);

	m->vertices[0] = vector3{ 1,1,1 };
	m->vertices[1] = vector3{ -1,1,1 };
	m->vertices[2] = vector3{ -1,-1,1 };
	m->vertices[3] = vector3{ 1,-1,1 };

	m->vertices[4] = vector3{ 1,1,-1 };
	m->vertices[5] = vector3{ -1,1,-1 };
	m->vertices[6] = vector3{ -1,-1,-1 };
	m->vertices[7] = vector3{ 1,-1,-1 };

	m->triangles[0] = 0; m->triangles[1] = 1; m->triangles[2] = 2;
	m->triangles[3] = 0; m->triangles[4] = 2; m->triangles[5] = 3;
	m->triangles[6] = 0; m->triangles[7] = 3; m->triangles[8] = 4;
	m->triangles[9] = 7; m->triangles[10] = 4; m->triangles[11] = 3;
	m->triangles[12] = 1; m->triangles[13] = 0; m->triangles[14] = 5;
	m->triangles[15] = 0; m->triangles[16] = 4; m->triangles[17] = 5;
	m->triangles[18] = 2; m->triangles[19] = 1; m->triangles[20] = 6;
	m->triangles[21] = 1; m->triangles[22] = 5; m->triangles[23] = 6;
	m->triangles[24] = 3; m->triangles[25] = 2; m->triangles[26] = 6;
	m->triangles[27] = 3; m->triangles[28] = 6; m->triangles[29] = 7;
	m->triangles[30] = 4; m->triangles[31] = 7; m->triangles[32] = 6;
	m->triangles[33] = 5; m->triangles[34] = 4; m->triangles[35] = 6;*/

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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, -0.5);
	//glRotatef(-35+90, 1, 0, 0);
	//glRotatef(45, 0, 0, 1);

	// -z into screen, ghmm
	//glPushMatrix();

	//gluPerspective(120, 1, 0.01, 10);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glFrontFace(GL_CW);

	glutMainLoop();
}

void spacegame::display()
{
	
	auto time_now = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(time_now - last_frame_time).count() / 1000000000.0f;
	last_frame_time = time_now;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	draw_mesh();

	glFlush();

	glutPostRedisplay();
}

void spacegame::mouse_move(int x, int y)
{
	int diff_x = x - last_mouse_x;
	int diff_y = y - last_mouse_y;
	
	glRotatef(diff_x * 0.5, 0, 0, 1);
	glRotatef(diff_y * 0.5, 1, 0, 0);

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
