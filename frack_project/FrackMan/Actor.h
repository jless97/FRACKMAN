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

class Actor : public GraphObject {
public:
  Actor(int image_id, int start_x, int start_y, Direction startDirection,
        float size, unsigned int depth, StudentWorld* world);
  virtual ~Actor();
  StudentWorld* returnWorld() const;
  virtual void do_something() = 0;
  virtual void get_annoyed(int how_much) = 0;
  virtual void set_bribe() = 0;
  virtual bool canActorsPassThroughMe() const;
  bool is_alive() const;
  void set_alive(bool value);
  bool moveDir(int direction, int numSquaresToMove);
  
private:
  StudentWorld* m_world;
  bool m_is_alive;
};

///////////////////////////////////////////////////////////////////////////
////////////////////-----------PROTESTER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

class Protester : public Actor {
public:
  Protester(int image_id, int start_x, int start_y, Direction startDirection, float size, unsigned int depth, StudentWorld* world, int health);
  virtual ~Protester();
  virtual void do_something() = 0;
  void doesTheSomething();
  virtual void get_annoyed(int how_much);
  virtual void set_bribe();
  int get_health() const;
  bool getLeaveOilFieldState() const;
  void setLeaveOilFieldState(bool value);
  void decHealth(int hitpoints);  ///
  void hitByBoulder();
  void set_dead();
  void changeDirectionToMove();
  bool canMove(int x, int y, int direction);
  void doMove(int x, int y, int direction);
  
private:
  int m_health;
  bool leaveField;
  int numMoves;
  int restingTicks;
  int newRestingTicks;
  int waitingTicksToShout;
  bool hasShouted;
  int shoutCounter;
};

///////////////////////////////////////////////////////////////////////////
////////////////-----------REGULAR PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

//IID_Protester
class RegularProtester : public Protester {
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
class HardcoreProtester : public Protester {
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
class Frackman : public Actor {
public:
  Frackman(int x, int y, StudentWorld* world);
  virtual ~Frackman();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  bool validPosition(int x, int y) const;
  int getSquirts() const;
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
class Dirt : public Actor {
public:
  Dirt(int x, int y, StudentWorld* world);
  virtual ~Dirt();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  virtual bool canActorsPassThroughMe() const;
  virtual void set_bribe();
  
private:
};

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------BOULDER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_BOULDER
class Boulder : public Actor {
public:
  Boulder(int x, int y, StudentWorld* world);
  virtual ~Boulder();
  virtual void do_something();
  virtual void get_annoyed(int how_much);
  bool is_dirtBelow() const;
  int getWaitingStateCounter() const;
  void moveBoulder();
  virtual bool canActorsPassThroughMe() const;
  virtual void set_bribe();
  
private:
  int m_state;
  int m_waitingStateCounter;
  
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------BARREL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//IID_BARREL
class Barrel : public Actor {
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
class Gold : public Actor {
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
class Bribe : public Actor {
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
class Sonar : public Actor {
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
class WaterSquirt : public Actor {
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
class WaterPool : public Actor {
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
