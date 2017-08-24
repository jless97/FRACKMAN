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
#include <iostream> /// DEBUGGING PURPOSES
using namespace std;

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------GLOBALS--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#define GRID_HEIGHT 64
#define GRID_WIDTH 64

//returns the lesser of two numbers
#define MIN(a, b) (a < b) ? (a) : (b)
//returns the greater of two numbers
#define MAX(a, b) (a > b) ? (a) : (b)

///////////////////////////////////////////////////////////////////////////
///////////////////-----------STUDENTWORLD--------------///////////////////
///////////////////////////////////////////////////////////////////////////

class StudentWorld : public GameWorld {
public:
  /* Standard Control Flow Functions */
  StudentWorld(std::string asset_dir);
  virtual int init();                                          // Initialize the FrackMan universe (and initial actor objects)
  virtual int move();                                          // Update the actor objects in the current level
  virtual void clean_up();                                     // Reset a level (due to a player death or completion of a level)
  void add_actor(Actor* actor);                                 // Add a new actor to StudentWorld
  void update_scoreboard(void);                                // Update the scoreboard display
  bool remove_dirt(Actor* a);                                  // Remove dirt occupied by an actor
  bool is_dirt_below(Actor* a);                                // Check if there is dirt directly below an actor
  bool boulder_hit_human(Actor* a);                            // Check if there is a human (currently frackman) below a boulder
  double radius(int x_1, int y_1, int x_2, int y_2) const;     // Returns the radius (Euclidean distance) between two objects
  bool radius_from_actor(int x, int y, double r, bool is_boulder) const; // Objects should not be placed within a certain distance of others
  int rand_int(int min, int max) const;                        // Generates a pseudo-random number in a given range
  void annoy_frackman(int how_much);                           // Decrease Frackman's health by a specified amount
  virtual ~StudentWorld();
  bool radius_from_boulder(int x, int y) const;

private:
  Dirt* m_dirt[GRID_WIDTH][GRID_HEIGHT];
  Frackman* m_frackman;
  // TESTING
  Boulder* m_boulder;
  std::vector<Actor*> m_actors;
  int m_nbarrels;
  int m_nticks_since_added_protester;
  int m_nprotesters;
  
  /* StudentWorld Private Functions */
  void init_dirt(void);
  void deinit_dirt(void);
};

#endif // STUDENTWORLD_H_
