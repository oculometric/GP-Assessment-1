#include "spacegame.h"

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#include "glut_callback_handlers.h"

#include <iostream>

spacegame::spacegame(int argc, char* argv[], unsigned int x, unsigned int y)
{
	glut_callback_handlers::init(this);

	last_frame_time = std::chrono::high_resolution_clock::now();

	v1_m = norm(vector2{ 0.5, 0.5 });
	v2_m = norm(vector2{ 0.5, 0.5 });
	v3_m = norm(vector2{ 0.5, 0.5 });

	glutInit(&argc, argv);
	glutInitWindowSize(x, y);
	glutCreateWindow("GP");
	glutDisplayFunc(glut_callback_handlers::display);
	glutMainLoop();
}

void spacegame::display()
{
	glClear(GL_COLOR_BUFFER_BIT);
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
	glFlush();

	// move vertices
	auto time_now = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration_cast<std::chrono::seconds>(time_now - last_frame_time).count();
	last_frame_time = time_now;
	v1 += v1_m * delta_time;
	v2 += v2_m * delta_time;
	v3 += v3_m * delta_time;

	std::cout << delta_time << std::endl;
}


spacegame::~spacegame()
{ }
