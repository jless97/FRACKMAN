#include "GameWorld.h"
#include "GameController.h"
#include <string>
#include <cstdlib>
using namespace std;

bool GameWorld::get_key(int& value) {
	bool gotKey = m_controller->get_last_key(value);

	if (gotKey) {
    if (value == 'q') {
			m_controller->quit_game();
    }
    else if (value == '\x03') { // CTRL-C
			exit(0);
    }
	}
  
	return gotKey;
}

void GameWorld::play_sound(int sound_id) {
	m_controller->play_sound(sound_id);
}

void GameWorld::set_game_stat_text(string text) {
	m_controller->set_game_stat_text(text);
}
