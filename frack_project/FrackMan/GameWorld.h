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
    // Constructor
    GameWorld(std::string assetDir)
     : m_lives(START_PLAYER_LIVES), m_score(0), m_level(0),
       m_controller(nullptr), m_assetDir(assetDir) {}
    // Destructor
    virtual ~GameWorld() {}
    // Pure Virtual Functions
    virtual int init() = 0;
    virtual int move() = 0;
    virtual void cleanUp() = 0;
    // Initialize the scoreboard
    void setGameStatText(std::string text);
    // Update gameworld, and play sounds (if necessary)
    bool getKey(int& value);
    void playSound(int soundID);
    /* Functions to be utilized by StudentWorld (and to update game) */
      // Get the current level
    UINT getLevel() const {
      return m_level;
    }
      // Get the current lives of the player
    UINT getLives() const {
      return m_lives;
    }
      // Decrement the player lives count
    void decLives() {
      m_lives--;
    }
      // Increment the player lives count
	  void incLives() {
		  m_lives++;
	  }
      // Get the current score of the player
    UINT getScore() const {
      return m_score;
    }
      // Increase the score of the player every clock tick (if necessary)
    void increaseScore(UINT howMuch) {
      m_score += howMuch;
    }
    /* NOTE: The following should be used by only the framework, not the student */
      // Check if the player has run out of lives
    bool isGameOver() const {
      return m_lives == 0;
    }
      // If the player has collected all of the oil barrels, advance to the next level
    void advanceToNextLevel() {
      ++m_level;
    }
      // Game engine stuff
    void setController(GameController* controller) {
      m_controller = controller;
    }
      // Access of the images and sound effects in the assets directory
    std::string assetDirectory() const {
      return m_assetDir;
    }
	
  private:
    UINT	m_lives;
    UINT	m_score;
    UINT	m_level;
    GameController* m_controller;
    std::string		m_assetDir;
};

#endif // GAMEWORLD_H_
