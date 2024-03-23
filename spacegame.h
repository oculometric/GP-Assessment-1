#pragma once

#include <chrono>

#include "vector2.h"
#include "mesh.h"

class SpaceGame
{
private:
	Vector2 v1{ 0.5f, 0.5f };
	Vector2 v2{ 0.2f, -0.8f };
	Vector2 v3{ -0.6f, 0.1f };

	Vector2 v1_m;
	Vector2 v2_m;
	Vector2 v3_m;

	Vector3 camera_rotation;
	Vector3 camera_position;
	Vector3 camera_local_velocity;

	int last_mouse_x = 0;
	int last_mouse_y = 0;
	std::chrono::steady_clock::time_point last_frame_time;

	static void moveAndBounceVertex(Vector2& v, Vector2& v_m, float f);

public:
	SpaceGame(int argc, char* argv[], unsigned int x, unsigned int y);

	int num_edges = 5;
	Mesh* m;

	void display();
	void mouseMove(int x, int y);
	void mouseMovePassive(int x, int y);
	void mouseClick(int button, int state, int x, int y);
	void keyDown(uint8_t key, int x, int y);
	void keyUp(uint8_t key, int x, int y);
	void frameRefresh(int value);

	void drawMesh();

	SpaceGame() = delete;

	~SpaceGame();
};