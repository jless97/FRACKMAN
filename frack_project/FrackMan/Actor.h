/*******************************************************************************
 *Actor.h
 *
 * This routine is responsible for all of the objects (actors) within the game
 * The main function of each actor is to do something each clock tick
 *
 * $LastChangedBy: jless $
 * $LastChangedDate: 2017-08-23 11:11:11 -0700 (Wed, 23 Aug 2017) $
 *
 ******************************************************************************/

#ifndef ACTOR_H_
#define ACTOR_H_

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------INCLUDES--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#include "GraphObject.h"

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------GLOBALS--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------ACTOR--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

class StudentWorld;

class Actor : public GraphObject {
public:
  Actor(int image_id, int start_x, int start_y, Direction start_dir, float image_size, unsigned int image_depth, StudentWorld* current_world);
  virtual void do_something() = 0;
  bool is_alive(void);
  void set_dead(void);
  StudentWorld* world(void);
  virtual ~Actor();
private:
  StudentWorld* m_world;
  bool m_alive;
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------DIRT--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_DIRT
class Dirt : public Actor {
public:
  Dirt(int start_x, int start_y, StudentWorld* world);
  virtual void do_something();
  virtual ~Dirt();
private:
};

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------BOULDER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_BOULDER
class Boulder : public Actor {
public:
  Boulder(int start_x, int start_y, StudentWorld* world);
  virtual void do_something();
  virtual ~Boulder();
private:
  int state;
  int m_nticks_before_fall;
  enum {
    stable = 0,
    waiting = 1,
    falling = 2
  } BOULDER_STATE;
  int m_state;
  
};

///////////////////////////////////////////////////////////////////////////
//////////////////-----------WATER SQUIRT--------------////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_WATER_SPURT
class WaterSquirt : public Actor {
public:
  WaterSquirt(int x, int y, StudentWorld* world, Direction dir);
  virtual void do_something(void);
  virtual ~WaterSquirt();
  
private:
  int m_nticks_before_vanish;
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------HUMAN--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

class Human : public Actor {
public:
  Human(int image_id, int start_x, int start_y, Direction start_dir, float image_size,
        unsigned int image_depth, StudentWorld* current_world, int start_health);
  virtual void do_something() = 0;
  void get_annoyed(int how_much);
  int get_health(void);
  virtual ~Human();
private:
  int m_health;
};

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------FRACKMAN--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_PLAYER
class Frackman : public Human {
public:
  Frackman(StudentWorld* world);
  virtual void do_something();
  int get_squirts(void);              // Get the number of water squirts in frackman's inventory
  int get_gold(void);                 // Get the number of gold nuggets in frackman's inventory
  int get_sonars(void);               // Get the number of sonar kits in frackman's inventory
  void update_gold(int how_much);     // Update frackman's gold count
  void update_sonar(int how_much);    // Update frackman's sonar kit count
  void update_water(int how_much);    // Update frackman's water count
  virtual ~Frackman();
  
private:
  int m_squirts;
  int m_sonars;
  int m_gold;
};

///////////////////////////////////////////////////////////////////////////
////////////////////-----------PROTESTER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

class Protester : public Human {
public:
private:
};

///////////////////////////////////////////////////////////////////////////
////////////////-----------REGULAR PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

//IID_Protester
class RegularProtester : public Protester {
public:
private:
};

///////////////////////////////////////////////////////////////////////////
///////////////-----------HARDCORE PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

//IID_HARD_CORE_Protester
class HardcoreProtester : public Protester {
public:
private:
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOODIE--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

class Goodie : public Actor {
public:
  Goodie(int image_id, int start_x, int start_y, Direction start_dir, float image_size,
         unsigned int image_depth, StudentWorld* world, int ticks);
  virtual void do_something(void) = 0;
  int get_remaining_ticks(void) const;          // Returns the remaining number of ticks before the goodie disappears
  void set_remaining_ticks(void);               // Set the ticks before disappearing (for sonar kit and water pool)
  void update_ticks(void);                      // Updates the ticks before the goodie disappears
  virtual ~Goodie();
  
private:
  int m_nticks_before_vanish;
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------BARREL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_BARREL
class Barrel : public Goodie {
public:
  Barrel(int start_x, int start_y, StudentWorld* world);
  virtual void do_something(void);
  virtual ~Barrel();
  
private:
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOLD--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_GOLD
class Gold : public Goodie {
public:
  Gold(int start_x, int start_y, StudentWorld* world, int state, bool is_visible);
  virtual void do_something(void);
  bool is_permanent_state(void);       // Return the state of the gold object
  void update_state(void);             // Update the state of the gold object (i.e. player places it down, switch to temporary)
  virtual ~Gold();
  
private:
  int m_state;
};

///////////////////////////////////////////////////////////////////////////
////////////////////-----------SONAR KIT--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_SONAR
class Sonar : public Goodie {
public:
  Sonar(StudentWorld* world);
  virtual void do_something(void);
  virtual ~Sonar();
  
private:
};

///////////////////////////////////////////////////////////////////////////
///////////////////-----------WATER POOL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_WATER_POOL
class WaterPool : public Goodie {
public:
  WaterPool(int start_x, int start_y, StudentWorld* world);
  virtual void do_something(void);
  virtual ~WaterPool();
  
private:
};

#endif // ACTOR_H_
