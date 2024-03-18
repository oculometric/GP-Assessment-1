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

	m.vertices = new vector3[8];
	m.vertices[0] = vector3{ 1,1,1 };
	m.vertices[1] = vector3{ -1,1,1 };
	m.vertices[2] = vector3{ -1,-1,1 };
	m.vertices[3] = vector3{ 1,-1,1 };

	m.vertices[4] = vector3{ 1,1,-1 };
	m.vertices[5] = vector3{ -1,1,-1 };
	m.vertices[6] = vector3{ -1,-1,-1 };
	m.vertices[7] = vector3{ 1,-1,-1 };

	m.triangles = new uint32_t[12*3]
	{ 
		0,1,2,
		0,2,3,
		0,3,4,
		3,4,7,
		1,0,5,
		0,4,5,
		2,1,6,
		1,5,6,
		3,2,6,
		3,6,7,
		4,7,6,
		5,4,6
	};

	m.num_tris = 12 * 3;
	m.num_verts = 8;


	glutInit(&argc, argv);
	glutInitWindowSize(x, y);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("GP");
	glutDisplayFunc(glut_callback_handlers::display);
	glutMotionFunc(glut_callback_handlers::mouse_move);
	glutPassiveMotionFunc(glut_callback_handlers::mouse_move_passive);
	glutMouseFunc(glut_callback_handlers::mouse_click);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, 0.5);
	glRotatef(35-90, 1, 0, 0);
	glRotatef(45, 0, 0, 1);

	glEnable(GL_DEPTH);
	glDepthFunc(GL_LESS);
	glEnable(GL_3D);

	glutMainLoop();
}

void spacegame::display()
{
	
	auto time_now = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(time_now - last_frame_time).count() / 1000000000.0f;
	last_frame_time = time_now;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/*
	glBegin(GL_POLYGON);
	{
		glColor3f(1, 0, 0);
		glVertex2f(v1.x, v1.y);
		glColor3f(0, 1, 0);
		glVertex2f(v2.x, v2.y);
		glColor3f(0, 0, 1);
		glVertex2f(v3.x, v3.y);
	}
	glEnd();

	// move vertices
	v1 += v1_m * delta_time;
	v2 += v2_m * delta_time;
	v3 += v3_m * delta_time;

	move_and_bounce_vertex(v1, v1_m, delta_time);
	move_and_bounce_vertex(v2, v2_m, delta_time);
	move_and_bounce_vertex(v3, v3_m, delta_time);
	*/
	

	glLineWidth(5);
	
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
	if (m.triangles == NULL || m.vertices == NULL) return;

	

	for (uint32_t i = 0; i < m.num_tris; i++)
	{
		if (i % 3 == 0) glBegin(GL_POLYGON);
		vector3 vert = m.vertices[m.triangles[i]] * 0.5f;
		glColor3f(vert.x + 0.5f, vert.y + 0.5f, vert.z + 0.5f);
		glVertex3f(vert.x * 0.5f, vert.y * 0.5f, vert.z * 0.5f);
		if (i % 3 == 2) glEnd();
	}
}


spacegame::~spacegame()
{ }
