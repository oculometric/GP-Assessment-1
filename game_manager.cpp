#include "game_manager.h"
#include "scene_manager.h"

void GameManager::init() { }

void GameManager::start(SceneManager* manager)
{
	scene_manager = manager;
}

void GameManager::update(float delta_time) { }

void GameManager::mouseMove(int delta_x, int delta_y, bool down) { }

void GameManager::mousePressed(int button, bool down, int pos_x, int pos_y) { }

void GameManager::keyPressed(unsigned char key, bool down) { }

void GameManager::destroy() { }

GameManager::GameManager()
{
	scene_manager = NULL;
}
