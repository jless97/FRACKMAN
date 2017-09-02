/*******************************************************************************
 *GameWorld.h
 *
 * This routine was created by Professor Smallberg to handle the game engine,
 * setting the frequency that the StudentWorld class is accessed and updated,
 * and accessing the assets (i.e. sound effects, images), and updating graphics
 *
 *
 ******************************************************************************/

#ifndef GAMEWORLD_H_
#define GAMEWORLD_H_

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------INCLUDES--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#include "GameConstants.h"
#include "globals.h"
#include <string>

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------GLOBALS--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

const int START_PLAYER_LIVES = 3;

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------GAMEWORLD--------------////////////////////
///////////////////////////////////////////////////////////////////////////

class GameController;

class GameWorld {
public:
  /* Standard Control Flow Functions */
  GameWorld(std::string asset_dir) :m_lives(START_PLAYER_LIVES),m_score(0),m_level(0),m_controller(nullptr),m_asset_dir(asset_dir){}
  virtual ~GameWorld() {}
  virtual int init() = 0; // Initialize the game world
  virtual int move() = 0; // Ask each actor to do something
  virtual void clean_up() = 0; // Clean up actors
  void set_game_stat_text(std::string text); // Initialize the scoreboard
  bool get_key(int& value); // Get user input from keyboard
  void play_sound(int sound_id); // Play object sound
  
  /* Functions to be utilized by StudentWorld (and to update game) */
  UINT get_level() const { return m_level; } // Get the current level
  UINT get_lives() const { return m_lives; } // Get the current lives of the player
  void dec_lives() { m_lives--; } // Decrement the player lives count
  void inc_lives() { m_lives++; } // Increment the player lives count
  UINT get_score() const { return m_score; } // Get the current score of the player
  void increase_score(UINT how_much) { m_score += how_much; } // Increase the score of the player every clock tick (if necessary)
  
  /* NOTE: The following should be used by only the framework, not the student */
  bool is_game_over() const { return m_lives == 0; } // Check if the player has run out of lives
  void advance_to_next_level() { ++m_level; } // If the player has collected all of the oil barrels, advance to the next level
  void set_controller(GameController* controller) { m_controller = controller; } // Game engine stuff
  std::string asset_directory() const { return m_asset_dir; } // Access of the images and sound effects in the assets directory
	
private:
  UINT	m_lives;
  UINT	m_score;
  UINT	m_level;
  GameController* m_controller;
  std::string		m_asset_dir;
};

#endif // GAMEWORLD_H_
