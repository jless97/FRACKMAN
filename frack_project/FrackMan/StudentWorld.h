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
///////////////////-----------STUDENTWORLD--------------///////////////////
///////////////////////////////////////////////////////////////////////////

// The game grid (i.e. the oil field)
class StudentWorld : public GameWorld {
public:
  // Constructor
  StudentWorld(std::string asset_dir);
  // Destructor
  ~StudentWorld();
  // GameWorld Virtual Functions
  virtual int init();
  virtual int move();
  virtual void clean_up();
  // StudentWorld Functions
  bool is_dirt(int x, int y) const;
  bool is_dirt(int x, int y, int direction) const;
  bool is_boulder(int x, int y, int direction) const;
  bool remove_dirt(int x, int y, int direction);
  void remove_dirt(int x, int y);
  void kill_frackman();
  void annoy_protester(Actor* a, int how_much);
  void annoy_frackman();
  void set_barrel(int how_much);
  void set_gold(int how_much);
  void set_bribe(int how_much);
  void add_bribe(Actor* a);
  void add_water_squirt(Actor* a, Actor::Direction direction);
  void set_sonars(int how_much);
  void set_squirts(int how_much);
  double radius(int x1, int y1, int x2, int y2) const;
  int rand_int(int min, int max);
  bool is_near_frackman(Actor* a, int radius) const;
  bool is_near_protester(Actor* a, int radius);
  bool is_near_boulder(int x, int y, int r);
  void is_near_actor(Actor* a, int r);
  bool facing_toward_frackman(Actor* a) const;
  GraphObject::Direction line_of_sight_to_frackman(Actor* a) const;
  //GraphObject::Direction DirToFrackMan(Actor* a) const;
  bool can_actor_move_to(Actor* a, int x, int y) const;
  int get_current_level() const;
  bool no_dirt(int start_x, int start_y) const;
  bool check_radius(int x, int y);
  
private:
  std::vector<Actor*> m_actor;
  FrackMan* m_frackman;
  Dirt* m_dirt[VIEW_WIDTH][VIEW_HEIGHT];
  void create_dirt();
  void add_initial_actors();
  void add_more_actors();
  bool is_placeable(int x, int y);
  void update_display_text();
  int m_currentLevel;
  int m_ticks;
  int m_barrels;
  int m_protesters;
  
};

#endif // STUDENTWORLD_H_
