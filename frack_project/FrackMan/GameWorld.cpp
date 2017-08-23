#include "GameWorld.h"
#include "GameController.h"
#include <string>
#include <cstdlib>
using namespace std;

bool GameWorld::get_key(int& value) {
	bool gotKey = m_controller->getLastKey(value);

	if (gotKey) {
    if (value == 'q') {
			m_controller->quitGame();
    }
    else if (value == '\x03') { // CTRL-C
			exit(0);
    }
	}
  
	return gotKey;
}

void GameWorld::play_sound(int soundID) {
	m_controller->play_sound(soundID);
}

void GameWorld::set_game_stat_text(string text) {
	m_controller->set_game_stat_text(text);
}
