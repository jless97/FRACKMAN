#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

#include "SpriteManager.h"
#include <string>
#include <map>
#include <iostream>
#include <sstream>

enum GameControllerState
{
	welcome, contgame, finishedlevel, init, clean_up, makemove, animate, gameover, prompt, quit, not_applicable
};

const int INVALID_KEY = 0;

class GraphObject;
class GameWorld;

class GameController
{
public:
	void run(int argc, char* argv[], GameWorld* gw, std::string window_title);
	bool get_last_key(int& value)
	{
		if (m_last_key_hit != INVALID_KEY)
		{
			value = m_last_key_hit;
			m_last_key_hit = INVALID_KEY;
			return true;
		}
		return false;
	}
	void play_sound(int sound_id);
	void set_game_stat_text(std::string text) { m_game_stat_text = text; }
	void do_something();
	void reshape(int w, int h);
	void keyboard_event(unsigned char key, int x, int y);
	void specialkeyboard_event(int key, int x, int y);
	void quit_game() { set_game_state(quit); }
	  // Meyers singleton pattern
	static GameController& get_instance() { static GameController instance; return instance; }

private:
	GameWorld*	        m_gw;
	GameControllerState	m_game_state;
	GameControllerState	m_next_state_after_prompt;
	GameControllerState	m_next_state_after_animate;
	int			            m_last_key_hit;
	bool		            m_singleStep;
	std::string         m_game_stat_text;
	std::string         m_main_message;
	std::string         m_second_message;
	int			            m_cur_intra_frame_tick;
	typedef std::map<int, std::string> SoundMapType;
	typedef std::map<int, std::string> DrawMapType;
	SoundMapType        m_sound_map;
	bool		            m_player_won;
	SpriteManager       m_sprite_manager;
  /* Private Functions */
	void set_game_state(GameControllerState s) { if (m_game_state != quit) { m_game_state = s; } }
	void init_drawers_and_sounds();
	void display_gameplay();
	void draw_dirt(double gx, double gy, double gz, double size);	// optimized - does not use sprite engine
};

inline GameController& Game() { return GameController::get_instance(); }

#endif // GAMECONTROLLER_H_
