/*******************************************************************************
 *StudentWorld.h
 *
 * This routine is responsible for setting up the configuration of the game
 * (i.e. (de)initializing actors, updating actors)
 *
 * $LastChangedBy: jless $
 * $LastChangedDate: 2017-08-23 11:11:11 -0700 (Wed, 23 Aug 2017) $
 *
 ******************************************************************************/

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------INCLUDES--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
using namespace std;

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------GLOBALS--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#define GRID_HEIGHT 64
#define GRID_WIDTH 64

///////////////////////////////////////////////////////////////////////////
///////////////////-----------STUDENTWORLD--------------///////////////////
///////////////////////////////////////////////////////////////////////////

class StudentWorld : public GameWorld {
public:
  /* Standard Control Flow Functions */
  StudentWorld(std::string asset_dir);
  virtual int init();                   // Initialize the FrackMan universe (and initial actor objects)
  virtual int move();                   // Update the actor objects in the current level
  virtual void clean_up();              // Reset a level (due to a player death or completion of a level)
  void update_scoreboard(void);         // Update the scoreboard display
  bool remove_dirt(Actor* a);           // Remove dirt occupied by an actor
  virtual ~StudentWorld();

private:
  Dirt* m_dirt[GRID_WIDTH][GRID_HEIGHT];
  Frackman* m_frackman;
  std::vector<Actor*> m_actors;
  int m_nbarrels;
  int m_nticks_since_added_protester;
  int m_nprotesters;
  
  /* StudentWorld Private Functions */
  void init_dirt(void);
  void deinit_dirt(void);
};

#endif // STUDENTWORLD_H_
