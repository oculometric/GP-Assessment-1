#include "museum_game.h"

#include <iostream>

using namespace std;

void MuseumGame::start()
{
	cout << "museum start" << endl;
}

void MuseumGame::update(float delta_time)
{
	cout << "museum update, " << delta_time << endl;
}

void MuseumGame::mouseMove(int delta_x, int delta_y, bool down)
{
	cout << "museum mouse" << endl;
}

void MuseumGame::keyPressed(unsigned char key, bool down)
{
	cout << "museum key " << key << endl;
}
