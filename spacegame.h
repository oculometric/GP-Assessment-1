#pragma once

#include <chrono>

#include "vector2.h"

class spacegame
{
private:
	vector2 v1{ 0.5f, 0.5f };
	vector2 v2{ 0.2f, -0.8f };
	vector2 v3{ -0.6f, 0.1f };

	vector2 v1_m;
	vector2 v2_m;
	vector2 v3_m;

	std::chrono::steady_clock::time_point last_frame_time;

	static void move_and_bounce_vertex(vector2& v, vector2& v_m, float f);

public:
	spacegame(int argc, char* argv[], unsigned int x, unsigned int y);

	void display();

	spacegame() = delete;

	~spacegame();
};