#include "scene_manager.h"
#include "space_game.h"
#include "museum_game.h"
#include "multi_game.h"

int main(int argc, char* argv[])
{
	SceneManager s;
	s.initialise(argc, argv, 600, 600);

	SpaceGame* sg = new SpaceGame();
	MuseumGame* mg = new MuseumGame();
	sg->init();
	mg->init();

	setGame(0, sg);
	setGame(1, mg);

	s.startMainloop(sg);
}