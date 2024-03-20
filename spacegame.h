#pragma once

#include <chrono>

#include "vector2.h"
#include "mesh.h"

class spacegame
{
private:
	vector2 v1{ 0.5f, 0.5f };
	vector2 v2{ 0.2f, -0.8f };
	vector2 v3{ -0.6f, 0.1f };

	vector2 v1_m;
	vector2 v2_m;
	vector2 v3_m;

	vector3 camera_rotation;
	vector3 camera_position;
	vector3 camera_local_velocity;

	int last_mouse_x = 0;
	int last_mouse_y = 0;
	std::chrono::steady_clock::time_point last_frame_time;

	static void move_and_bounce_vertex(vector2& v, vector2& v_m, float f);

public:
	spacegame(int argc, char* argv[], unsigned int x, unsigned int y);

	int num_edges = 5;
	mesh* m;

	void display();
	void mouse_move(int x, int y);
	void mouse_move_passive(int x, int y);
	void mouse_click(int button, int state, int x, int y);
	void key_down(uint8_t key, int x, int y);
	void key_up(uint8_t key, int x, int y);

	void draw_mesh();

	spacegame() = delete;

	~spacegame();
};