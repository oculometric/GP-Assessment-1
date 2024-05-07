#include "scene_manager.h"
#include "space_game.h"

int main(int argc, char* argv[])
{
	SceneManager s(argc, argv, 600, 600, new SpaceGame());
}