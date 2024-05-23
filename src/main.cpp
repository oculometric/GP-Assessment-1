#include "../include/scene_manager.h"
#include "../include/space_game.h"
#include "../include/museum_game.h"
#include "../include/multi_game.h"

int main(int argc, char* argv[])
{
	// init the scene manager (configures GLUT and openGL)
	SceneManager s;
	s.initialise(argc, argv, 640, 480);

	// init the two game managers for the different scenes (loads their resources)
	SpaceGame* sg = new SpaceGame();
	MuseumGame* mg = new MuseumGame();
	sg->init();
	mg->init();

	// tell the multi-game manager about these two games
	setGame(0, sg);
	setGame(1, mg);

	// start the GLUT mainloop with the space game to start with
	s.startMainloop(sg);
}