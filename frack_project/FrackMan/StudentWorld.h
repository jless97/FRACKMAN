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

// Oil field grid dimensions
#define GRID_HEIGHT 64
#define GRID_WIDTH 64

// Extra live point value
#define EXTRA_LIVE 50000

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
  void add_actor(Actor* actor);                                // Add a new actor to StudentWorld
  void add_initial_actors(void);                               // Add initial actors to the oil field (i.e. boulders, oil barrels, gold)
  void add_additional_actors(void);                            // Add additional actors to oil field (i.e. protesters, sonars, water pools)
  void update_scoreboard(void);                                // Update the scoreboard display
  void dec_barrels(void);                                      // Decrement barrel count
  void update_gold_count(void);                                // Increment gold count of frackman
  void update_sonar_count(void);                               // Increment sonar kit count of frackman
  void update_water_count(void);                               // Increment water count of frackman
  void illuminate_goodies(void);                               // If frackman uses a sonar kit, illuminate goodies within radius of 12
  void set_bribe(int x, int y);                                // Set a bribe for a protester
  void set_squirt(int x, int y, GraphObject::Direction dir);   // Set a water squirt onto the oil field
  bool remove_dirt(Actor* a);                                  // Remove dirt occupied by an actor
  bool is_dirt(Actor* a, GraphObject::Direction start_dir);    // Check if there is dirt directly next to an actor in a specified direction
  bool is_dirt_in_square(int x, int y);                        // Check if there is dirt in a 4x4 square (for placement of water pool)
  bool boulder_hit_human(Actor* a);                            // Check if there is a human (currently frackman) below a boulder
  int radius(int x_1, int y_1, int x_2, int y_2) const;        // Returns the radius (Euclidean distance) between two objects
  // Objects should not be placed within a certain distance of others
  bool radius_from_actor(int x, int y, double r, bool is_boulder = false, bool is_frackman = false, bool is_protester = false) const;
  int rand_int(int min, int max) const;                        // Generates a pseudo-random number in a given range
  // Generates the coordinates of the actor to be inserted into the oil field
  void generate_coordinates(int x_min, int x_max, int y_min, int y_max, int shaft_y_coord, int* x_coord, int* y_coord, bool is_water=false);
  void annoy_frackman(int how_much);                           // Decrease Frackman's health by a specified amount
  virtual ~StudentWorld();

private:
  Dirt* m_dirt[GRID_WIDTH][GRID_HEIGHT];
  Frackman* m_frackman;
  // TESTING
  std::vector<Actor*> m_actors;
  int m_nbarrels;
  int m_nticks_since_added_protester;
  int m_nprotesters;
  
  /* StudentWorld Private Functions */
  void init_dirt(void);
  void deinit_dirt(void);
};

#endif // STUDENTWORLD_H_
