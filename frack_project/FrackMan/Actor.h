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

const int STABLE_STATE = 1;
const int WAITING_STATE = 2;
const int FALLING_STATE = 3;


///////////////////////////////////////////////////////////////////////////
///////////////////////-----------ACTOR--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

class StudentWorld;

class Actor : public GraphObject //abstract base class
{
public:
  
  Actor(int image_id, int start_x, int start_y, Direction start_direction,
        float size, unsigned int depth, StudentWorld* world);
  virtual ~Actor();
  StudentWorld* return_world() const;
  virtual void do_something() = 0;
  virtual void get_annoyed(int how_much) = 0;
  virtual void set_bribe() = 0;
  virtual bool can_actors_pass_through_me() const;
  bool is_alive() const;
  void set_alive(bool value);
  bool move_dir(int direction, int num_squares_to_move);
  
private:
  StudentWorld* m_world;
  bool m_is_alive;
};


///////////////////////////////////////////////////////////////////////////
////////////////////-----------PROTESTER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

class Protester : public Actor //abstract base class
{
public:
  Protester(int image_id, int start_x, int start_y, Direction start_direction, float size, unsigned int depth, StudentWorld* world, int health);
  virtual ~Protester();
  virtual void do_something() = 0;
  void does_the_something();
  virtual void get_annoyed(int how_much);
  virtual void set_bribe();
  int get_health() const;
  bool get_leave_oil_field_state() const;
  void set_leave_oil_field_state(bool value);
  void dec_health(int hitpoints);  ///
  void hit_by_boulder();
  void set_dead();
  void change_direction_to_move();
  bool can_move(int x, int y, int direction);
  void do_move(int x, int y, int direction);
  
private:
  int m_health;
  bool leave_field;
  int num_moves;
  int resting_ticks;
  int new_resting_ticks;
  int waiting_ticks_to_shout;
  bool has_shouted;
  int shout_counter;
};

///////////////////////////////////////////////////////////////////////////
////////////////-----------REGULAR PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

//IID_Protester
class RegularProtester : public Protester
{
public:
  RegularProtester(int x, int y, StudentWorld* world);
  virtual ~RegularProtester();
  virtual void do_something();
  
private: ///
};

///////////////////////////////////////////////////////////////////////////
///////////////-----------HARDCORE PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

//IID_HARD_CORE_Protester
class HardcoreProtester : public Protester
{
public:
  HardcoreProtester(int x, int y, StudentWorld* world);
  virtual ~HardcoreProtester();
  virtual void do_something();
  virtual void set_bribe();
  
private: ///
};

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------FRACKMAN--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_PLAYER
class FrackMan : public Actor
{
public:
  FrackMan(int x, int y, StudentWorld* world);
  virtual ~FrackMan();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  bool valid_position(int x, int y) const;
  int get_squirts() const;
  int get_sonars() const;
  int get_gold() const;
  int get_health() const;
  void set_squirts(int how_much);
  void set_sonars(int how_much);
  void set_gold(int how_much);
  void set_dead();
  virtual void set_bribe();
  
private:
  int m_squirts;
  int m_sonars;
  int m_gold;
  int m_health;
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------DIRT--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_DIRT
class Dirt : public Actor
{
public:
  Dirt(int x, int y, StudentWorld* world);
  virtual ~Dirt();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  virtual bool can_actors_pass_through_me() const;
  virtual void set_bribe();
  
private:
};

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------BOULDER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_BOULDER
class Boulder : public Actor
{
public:
  Boulder(int x, int y, StudentWorld* world);
  virtual ~Boulder();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  bool is_dirt_below() const;
  int get_waiting_state_counter() const;
  void move_boulder();
  virtual bool can_actors_pass_through_me() const;
  virtual void set_bribe();
  
private:
  int m_state;
  int m_waiting_state_counter;
  
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------BARREL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_BARREL
class Barrel : public Actor
{
public:
  Barrel(int x, int y, StudentWorld* world);
  virtual ~Barrel();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  virtual void set_bribe();
  
private:
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOLD--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_GOLD
class Gold : public Actor
{
public:
  Gold(int x, int y, StudentWorld* world);
  virtual ~Gold();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  virtual void set_bribe();
  
private:
};

///////////////////////////////////////////////////////////////////////////
////////////////////////-----------BRIBE--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_GOLD
class Bribe : public Actor
{
public:
  Bribe(int x, int y, StudentWorld* world);
  virtual ~Bribe();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  virtual void set_bribe();
  
private:
  int m_ticks;
};

///////////////////////////////////////////////////////////////////////////
////////////////////-----------SONAR KIT--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_SONAR
class Sonar : public Actor
{
public:
  Sonar(int x, int y, StudentWorld* world);
  virtual ~Sonar();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  virtual void set_bribe();
  
private:
  int m_ticks;
};

///////////////////////////////////////////////////////////////////////////
//////////////////-----------WATER SQUIRT--------------////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_WATER_SPURT
class WaterSquirt : public Actor
{
public:
  WaterSquirt(int x, int y, Direction direction, StudentWorld* world);
  virtual ~WaterSquirt();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  virtual void set_bribe();
  
private:
  int m_distance;
};

///////////////////////////////////////////////////////////////////////////
///////////////////-----------WATER POOL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_WATER_POOL
class WaterPool : public Actor
{
public:
  WaterPool(int x, int y, StudentWorld* world);
  virtual ~WaterPool();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  virtual void set_bribe();
  
private:
  int m_ticks;
};

#endif // ACTOR_H_
