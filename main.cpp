#include "scene_manager.h"
#include "space_game.h"
#include "museum_game.h"

int main(int argc, char* argv[])
{
	SceneManager s;
	s.initialise(argc, argv, 600, 600);

	SpaceGame* sg = new SpaceGame();
	MuseumGame* mg = new MuseumGame();
	sg->init();
	mg->init();

	s.startMainloop(sg);
}