/*******************************************************************************
 *Actor.cpp
 *
 * This routine is responsible for all of the objects (actors) within the game
 * The main function of each actor is to do something each clock tick
 *
 * $LastChangedBy: jless $
 * $LastChangedDate: 2017-08-23 11:11:11 -0700 (Wed, 23 Aug 2017) $
 *
 ******************************************************************************/

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------INCLUDES--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <algorithm>
using namespace std;

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------ACTOR--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

Actor::Actor(int image_id, int start_x, int start_y, Direction startDirection,
             float size, unsigned int depth, StudentWorld* world)
:GraphObject(image_id, start_x, start_y, startDirection, size, depth),
m_world(world), m_is_alive(true) { set_visible(false); }

Actor::~Actor() { m_is_alive = false; set_visible(false); }

StudentWorld* Actor::returnWorld() const { return m_world; }

bool Actor::is_alive() const {
  if (m_is_alive) {
    return true;
  }
  
  return false;
}

void Actor::set_alive(bool value) { m_is_alive = false; }

bool Actor::canActorsPassThroughMe() const { return false; }

bool Actor::moveDir(int direction, int numMoves) {
  int x = get_x();
  int y = get_y();
  
  //Check to see if I'm allowed to move in a given direction
  if (returnWorld()->is_boulder(x, y, direction)) {
    return false;
  }
  
  if (returnWorld()->is_dirt(x, y, direction)) {
    return false;
  }
  
  if (!returnWorld()->can_actor_move_to(this, x, y)) {
    return false;
  }
  
  //If so, then move in the specified direction
  switch (direction) {
    case GraphObject::up:
      move_to(x, y + numMoves);
      break;
    case GraphObject::down:
      move_to(x, y - numMoves);
      break;
    case GraphObject::right:
      move_to(x + numMoves, y);
      break;
    case GraphObject::left:
      move_to(x - numMoves, y);
      break;
    default:
      break;
  }
  
  return true;
}

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------FRACKMAN--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Frackman::Frackman(int x, int y, StudentWorld* world)
:Actor(IID_PLAYER, x, y, right, 1.0, 0, world), m_squirts(5), m_sonars(1), m_gold(0), m_health(10) { set_visible(true); }

Frackman::~Frackman() {}

void Frackman::do_something() {
  //Check to see if Frackman is alive
  if (!is_alive()) {
    return;
  }
  
  int x = get_x();
  int y = get_y();
  returnWorld()->remove_dirt(x, y); //If Frackman spawns in a dirt location, remove the dirt
  int ch;
  StudentWorld* world = returnWorld();
  if (validPosition(x, y) == false) {
    return;
  }
  
  //Check to see if a valid key was input by the user
  if (returnWorld()->get_key(ch) == true) {
    //To see what Frackman is going to do in the given turn (tick)
    switch (ch) {
      case KEY_PRESS_LEFT: //Direction checks to see if Frackman can move in given direction
        if (get_direction() == left) {
          if (x != 0) {
            if (!world->isNearBoulder(x - 1, y, 3)) {
              move_to(x - 1, y);
              if (world->remove_dirt(x - 1, y, KEY_PRESS_LEFT)) {
                world->play_sound(SOUND_DIG);
              }
            }
            else if (world->can_actor_move_to(this, x - 1, y)) {
              set_direction(left);
            }
          }
          if (x == 0) {
            move_to(x, y);
          }
        }
        else {
          set_direction(left);
        }
        break;
      case KEY_PRESS_RIGHT:
        if (get_direction() == right) {
          if (x != 60) {
            if (!world->isNearBoulder(x + 1, y, 3)) {
              move_to(x + 1, y);
              if (world->remove_dirt(x + 1, y, KEY_PRESS_RIGHT)) {
                world->play_sound(SOUND_DIG);
              }
            }
            else if (world->can_actor_move_to(this, x + 1, y)) {
              set_direction(right);
            }
          }
          if (x == 60) {
            move_to(x, y);
          }
        }
        else {
          set_direction(right);
        }
        break;
      case KEY_PRESS_DOWN:
        if (get_direction() == down) {
          if (y != 0) {
            if (!world->isNearBoulder(x, y - 1, 3)) {
              move_to(x, y - 1);
              if (world->remove_dirt(x, y - 1, KEY_PRESS_DOWN)) {
                world->play_sound(SOUND_DIG);
              }
            }
            else if (world->can_actor_move_to(this, x, y - 1)) {
              set_direction(down);
            }
          }
          if (y == 0) {
            move_to(x, y);
          }
        }
        else {
          set_direction(down);
        }
        break;
      case KEY_PRESS_UP:
        if (get_direction() == up) {
          if (y != 60) {
            if (!world->isNearBoulder(x, y + 1, 3)) {
              move_to(x, y + 1);
              if (world->remove_dirt(x, y + 1, KEY_PRESS_UP)) {
                world->play_sound(SOUND_DIG);
              }
            }
            else if (world->can_actor_move_to(this, x, y + 1)) {
              set_direction(up);
            }
          }
          if (y == 60) {
            move_to(x, y);
          }
        }
        else {
          set_direction(up);
        }
        break;
      case KEY_PRESS_SPACE: //Shoots a squirt object (by creating a squirt object in oil field)
        if (m_squirts >= 1) {
          m_squirts--;
          returnWorld()->add_water_squirt(this, get_direction());
          returnWorld()->play_sound(SOUND_PLAYER_SQUIRT);
        }
        break;
      case KEY_PRESS_TAB: //Drops gold (by creating a bribe object in oil field)
        if (m_gold >= 1) {
          m_gold--;
          returnWorld()->add_bribe(this);
        }
        break;
      case KEY_PRESS_ESCAPE: //resets the level (or if no lives left, ends game)
        set_dead();
        break;
      case 'Z': //Uses a sonar to check for goodies in surrounding region
      case 'z':
        if (m_sonars >= 1) {
          m_sonars--;
          returnWorld()->is_near_actor(this, 12);
          returnWorld()->play_sound(SOUND_SONAR);
        }
        break;
      default:
        break;
    }
  }
}

void Frackman::get_annoyed(int how_much) {
  m_health -= 2;
  if (m_health <= 0) {
    set_dead();
    returnWorld()->play_sound(SOUND_PLAYER_GIVE_UP);
  }
}

bool Frackman::validPosition(int x, int y) const {
  if (x < 0 || x > 60 || y < 0 || y > 60) {
    return false;
  }
  
  return true;
}

int Frackman::getSquirts() const { return m_squirts; }

int Frackman::get_sonars() const { return m_sonars; }

int Frackman::get_gold() const { return m_gold; }

int Frackman::get_health() const { return m_health; }

void Frackman::set_squirts(int how_much) { m_squirts += how_much; }

void Frackman::set_sonars(int how_much) { m_sonars += how_much; }

void Frackman::set_gold(int how_much) { m_gold += how_much; }

void Frackman::set_dead() {
  returnWorld()->play_sound(SOUND_PLAYER_GIVE_UP);
  set_visible(false);
  m_health = 0;
  set_alive(false);
}

void Frackman::set_bribe() {}

///////////////////////////////////////////////////////////////////////////
////////////////////-----------PROTESTER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Protester::Protester(int image_id, int start_x, int start_y, Direction startDirection, float size,
                     unsigned int depth, StudentWorld* world, int health)
:Actor(image_id, start_x, start_y, startDirection, size, depth, world), m_health(health) {
  numMoves = world->rand_int(8, 60);
  restingTicks = max(0, 3 - world->getCurrentLevel() / 4);
  waitingTicksToShout = 15;
  leaveField = false;
  set_visible(true);
}

Protester::~Protester() {}

int Protester::get_health() const { return m_health; }

bool Protester::getLeaveOilFieldState() const { return leaveField == true; }


void Protester::decHealth(int hitpoints) { m_health -= hitpoints; }

void Protester::set_dead() {
  if (get_id() == IID_PLAYER) {
    returnWorld()->play_sound(SOUND_PLAYER_GIVE_UP);
  }
  else if (get_id() == IID_HARD_CORE_PROTESTER || get_id() == IID_PROTESTER) {
    returnWorld()->play_sound(SOUND_PROTESTER_GIVE_UP);
  }
  
  set_visible(false);
  m_health = 0;
  set_alive(false);
}

void Protester::set_bribe() { leaveField = true; }

void Protester::setLeaveOilFieldState(bool value) { leaveField = value; }

void Protester::doesTheSomething() {
  int x = get_x();
  int y = get_y();
  
  if (!is_alive()) {
    return;
  }
  if (leaveField) {
    restingTicks = 0;
  }
  
  // rest state
  if (restingTicks > 0 && newRestingTicks > 0) {
    restingTicks--;
    newRestingTicks--;
    return;
  }
  if (restingTicks > 0) {
    restingTicks--;
    return;
  }
  if (newRestingTicks > 0) {
    newRestingTicks--;
    return;
  }
  if (restingTicks == 0) {
    restingTicks = max(0, 3 - returnWorld()->getCurrentLevel() / 4);
  }
  
  waitingTicksToShout--;
  
  if (leaveField) {
    if (get_x() == 60 && get_y() == 60) { // exit point
      set_dead();
      restingTicks = INT_MAX;
      return;
    }
    // move one square closer to its exit point(at x = 60, y = 60)
    set_direction(up);
    if (returnWorld()->can_actor_move_to(this, get_x(), get_y())) {
      doMove(x, y, get_direction());
    }
    else {
      set_direction(right);
      doMove(x, y, get_direction());
    }
    
    restingTicks = max(0, 3 - returnWorld()->getCurrentLevel() / 4);
    return;
  }
  
  //If the protestor is within the proper distance of the Frackman, then annoy him
  //Reset the ticks to wait before you can shout again, and reset resting ticks
  if (returnWorld()->is_near_Frackman(this, 4) &&
      returnWorld()->facingTowardFrackman(this) &&
      waitingTicksToShout <= 0) {
    returnWorld()->play_sound(SOUND_PROTESTER_YELL);
    returnWorld()->annoy_frackman();
    waitingTicksToShout = 15;
    newRestingTicks = max(50, 100 - returnWorld()->getCurrentLevel() * 10);
    return;
  }
  
  //If the protester is right next to the Frackman, then he should stop moving
  if (returnWorld()->is_near_Frackman(this, 3)) {
    return;
  }
  
  /// if Frackman is in line of sight, move 1 toward
  
  GraphObject::Direction dir = returnWorld()->lineOfSightToFrackman(this);
  
  if (dir != GraphObject::none && !returnWorld()->is_near_Frackman(this, 3) ) {
    // face Frackman and take a step towards him if possible
    if (canMove(x, y, dir)) {
      set_direction(dir);
      doMove(x, y, dir);
      numMoves = 0;
      return;
    }
  }
  
  if (numMoves > 0 && canMove(x, y, get_direction())) {
    doMove(x, y, get_direction());
    return;
  }
  else {
    //If number of moves is 0, and you can't move in a given direction, change direction
    changeDirectionToMove();
  }
  
  return;
}

void Protester::get_annoyed(int how_much) {
  decHealth(how_much);
  
  //If dead, then prepare to leave the oil field
  if (get_health() <= 0) {
    leaveField = true;
    returnWorld()->play_sound(SOUND_PROTESTER_GIVE_UP);
    restingTicks = 0;
    returnWorld()->increase_score(100);
  }
  //Just annoy the protester
  else {
    returnWorld()->play_sound(SOUND_PROTESTER_ANNOYED);
    restingTicks = max(50, 100 - returnWorld()->getCurrentLevel() * 10);
  }
}

void Protester::changeDirectionToMove() {
  int x = get_x();
  int y = get_y();
  numMoves = 0;
  Direction direction = get_direction();
  do {
    int randNum = rand()%4;
    if (randNum == 0) {
      direction = left;
    }
    if (randNum == 1) {
      direction = right;
    }
    if (randNum == 2) {
      direction = up;
    }
    if (randNum == 3) {
      direction = down;
    }
    //keep changind directions, until a valid direction is found
  } while (canMove(x,y,direction) == false);
  set_direction(direction);
  numMoves = returnWorld()->rand_int(8, 60); //reset the number of moves
}

bool Protester::canMove(int x, int y, int direction) {
  //Check to see if the protester can move in a given direction
  if (direction == left){
    if (returnWorld()->can_actor_move_to(this, x - 1, y)) {
      return true;
    }
  }
  if (direction == right) {
    if (returnWorld()->can_actor_move_to(this, x + 1, y)) {
      return true;
    }
  }
  if (direction == up) {
    if (returnWorld()->can_actor_move_to(this, x, y + 1)) {
      return true;
    }
  }
  if (direction == down) {
    if (returnWorld()->can_actor_move_to(this, x, y - 1)) {
      return true;
    }
  }
  
  return false;
}

void Protester::doMove(int x, int y, int direction) {
  //If you can move in a given direction, then move in that direction
  if (direction == left) {
    if (canMove(x - 1, y, left)) {
      move_to(x - 1, y);
    }
  }
  if (direction == right) {
    if (canMove(x - 1, y, right)) {
      move_to(x + 1, y);
    }
  }
  if (direction == up) {
    if (canMove(x, y + 1, up)) {
      move_to(x, y + 1);
    }
  }
  if (direction == down) {
    if (canMove(x, y - 1, down)) {
      move_to(x, y - 1);
    }
  }
}

///////////////////////////////////////////////////////////////////////////
////////////////-----------REGULAR PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

RegularProtester::RegularProtester(int x, int y, StudentWorld* world)
:Protester(IID_PROTESTER, x, y, left, 1.0, 0, world, 5) {}

RegularProtester::~RegularProtester() {}

//Call protester's method to initiate the move
void RegularProtester::do_something() { doesTheSomething(); }

///////////////////////////////////////////////////////////////////////////
///////////////-----------HARDCORE PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

HardcoreProtester::HardcoreProtester(int x, int y, StudentWorld* world)
:Protester(IID_HARD_CORE_PROTESTER, x, y, left, 1.0, 0, world, 20) {}

HardcoreProtester::~HardcoreProtester() {}

//Call the protester's method to intiate the move
void HardcoreProtester::do_something() { doesTheSomething(); }

void HardcoreProtester::set_bribe() { setLeaveOilFieldState(false); }

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------BOULDER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Boulder::Boulder(int x, int y, StudentWorld* world)
:Actor(IID_BOULDER, x, y, down, 1.0, 1, world), m_state(STABLE_STATE), m_waitingStateCounter(0)
{ set_visible(true); }

Boulder::~Boulder() {}

int Boulder::getWaitingStateCounter() const { return m_waitingStateCounter; }

bool Boulder::is_dirtBelow() const {
  //Check to see if there is dirt below the boulder
  int x = get_x();
  int y = get_y();
  
  bool dirtBelow = false;
  for (int i = 0; i < 4; i++) {
    if (y != 0 && returnWorld()->is_dirt(x + i, y - 1)) {
      dirtBelow = true;
    }
  }
  
  if (!dirtBelow) {
    return false;
  }
  
  return true;
}

void Boulder::moveBoulder() {
  bool FrackmanBelow = false;
  bool ProtesterBelow = false;
  bool boulderBelow = false;
  
  int x = get_x();
  int y = get_y();
  
  move_to(x, y - 1);
  
  //Check to see if the Frackman, protester, or another boulder is below the boulder
  if (returnWorld()->is_near_Frackman(this, 3)) {
    FrackmanBelow = true;
  }
  if (is_dirtBelow() || boulderBelow == true || y == -1) {
    set_alive(false);
  }
  if (ProtesterBelow == true) {
    returnWorld()->annoy_protester(this, 20);
    returnWorld()->increase_score(500);
  }
  if (FrackmanBelow == true) {
    returnWorld()->kill_frackman();
  }
}

void Boulder::do_something() {
  if (!is_alive()) {
    return;
  }
  
  //Initial state when there is dirt below the boulder
  if (m_state == STABLE_STATE) {
    if (!is_dirtBelow()) {
      m_state = WAITING_STATE;
      m_waitingStateCounter++;
    }
    else {
      return;
    }
  }
  //If dirt was removed below the boulder, change to waiting state
  if (m_state == WAITING_STATE) {
    if (getWaitingStateCounter() == 30) {
      m_state = FALLING_STATE;
      returnWorld()->play_sound(SOUND_FALLING_ROCK);
    }
    else
      m_waitingStateCounter++;
  }
  //If the boulder is in a falling state check to see if it can move down
  if (m_state == FALLING_STATE) {
    moveBoulder();
  }
}

void Boulder::get_annoyed(int how_much) {}

bool Boulder::canActorsPassThroughMe() const { return false; }

void Boulder::set_bribe() {}

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------BARREL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Barrel::Barrel(int x, int y, StudentWorld* world)
:Actor(IID_BARREL, x, y, right, 1.0, 2, world) {}

Barrel::~Barrel() {}

void Barrel::do_something() {
  if (!is_alive()) {
    return;
  }
  
  //Activate visibility
  if (!is_visible() && returnWorld()->is_near_Frackman(this, 4)) {
    set_visible(true);
    return;
  }
  //Frackman picks up barrel
  if (returnWorld()->is_near_Frackman(this, 3)) {
    set_alive(false);
    returnWorld()->set_barrel(-1);
    returnWorld()->play_sound(SOUND_FOUND_OIL);
    returnWorld()->increase_score(1000);
    return;
  }
  
}

void Barrel::get_annoyed(int how_much) {}

void Barrel::set_bribe() {}

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOLD--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

Gold::Gold(int x, int y, StudentWorld* world)
:Actor(IID_GOLD, x, y, right, 1.0, 2, world) {}

Gold::~Gold() {}

void Gold::do_something() {
  if (!is_alive()) {
    return;
  }
  //Activate visibility
  if (!is_visible() && returnWorld()->is_near_Frackman(this, 4)) {
    set_visible(true);
    return;
  }
  //Frackman picks up gold
  if (returnWorld()->is_near_Frackman(this, 3)) {
    set_alive(false);
    set_visible(false);
    returnWorld()->play_sound(SOUND_GOT_GOODIE);
    returnWorld()->increase_score(10);
    returnWorld()->set_gold(1);
  }
}

void Gold::get_annoyed(int how_much) {}

void Gold::set_bribe() {}

///////////////////////////////////////////////////////////////////////////
////////////////////////-----------BRIBE--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Bribe::Bribe(int x, int y, StudentWorld* world)
:Actor(IID_GOLD, x, y, right, 1.0, 2, world) { set_visible(true); m_ticks = 100; }

void Bribe::do_something() {
  if (!is_alive()) {
    return;
  }
  m_ticks--;
  if (m_ticks <= 0) {
    set_alive(false);
    set_visible(false);
    return;
  }
  //Protester picks up bribe
  if (returnWorld()->is_near_protester(this, 3)) {
    set_alive(false);
    returnWorld()->play_sound(SOUND_PROTESTER_FOUND_GOLD);
    returnWorld()->increase_score(25);
    returnWorld()->set_bribe(1);
  }
}

void Bribe::get_annoyed(int how_much) {}

Bribe::~Bribe() {}

void Bribe::set_bribe() {}


///////////////////////////////////////////////////////////////////////////
////////////////////-----------SONAR KIT--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Sonar::Sonar(int x, int y, StudentWorld* world)
:Actor(IID_SONAR, x, y, right, 1.0, 2, world) {
  set_visible(true);
  m_ticks = max(100, 300 - 10 * returnWorld()->getCurrentLevel());
}

Sonar::~Sonar() {}

void Sonar::do_something() {
  if (!is_alive()) {
    return;
  }
  m_ticks--;
  //If ticks expire, remove from oil field
  if (m_ticks <= 0) {
    set_alive(false);
    set_visible(false);
    return;
  }
  //Frackman picks up sonar
  if (returnWorld()->is_near_Frackman(this, 3)) {
    set_alive(false);
    returnWorld()->play_sound(SOUND_GOT_GOODIE);
    returnWorld()->set_sonars(1);
    returnWorld()->increase_score(75);
  }
}

void Sonar::get_annoyed(int how_much) {}

void Sonar::set_bribe() {}

///////////////////////////////////////////////////////////////////////////
//////////////////-----------WATER SQUIRT--------------////////////////////
///////////////////////////////////////////////////////////////////////////

WaterSquirt::WaterSquirt(int x, int y, Direction direction, StudentWorld* world)
:Actor(IID_WATER_SPURT, x, y, direction, 1.0, 1, world), m_distance(5) { set_visible(true); }

WaterSquirt::~WaterSquirt() {}

void WaterSquirt::do_something() {
  int direction = get_direction();
  StudentWorld* world = returnWorld();
  int x = get_x();
  int y = get_y();
  bool is_dirt = false;
  
  if (!is_alive()) {
    return;
  }
  
  //If next to protester, stop moving
  if (returnWorld()->is_near_protester(this, 3)) {
    returnWorld()->annoy_protester(this, 2);
    set_alive(false);
    set_visible(false);
  }
  
  //If squirt traveled its distance, then remove squirt
  if (m_distance == 0) {
    set_alive(false);
    set_visible(false);
  }
  
  //Check to see if squirt can move in the given direction
  switch (direction) {
    case left:
      if (world->is_dirt(x, y, left) || (x < 0)) {
        is_dirt = true;
      }
      break;
    case right:
      if (world->is_dirt(x, y, right) || (x > 60)) {
        is_dirt = true;
      }
      break;
    case down:
      if (world->is_dirt(x, y, down) || (y < 0)) {
        is_dirt = true;
      }
      break;
    case up:
      if (world->is_dirt(x, y, up) || (y > 60)) {
        is_dirt = true;
      }
      break;
  }
  if (is_dirt == true || returnWorld()->isNearBoulder(get_x(), get_y(), 3)) {
    set_alive(false);
    set_visible(false);
  }
  
  else {
    moveDir(get_direction(), 1); //If the squirt moves, then reduce it's allowed travel distance
    m_distance--;
  }
}

void WaterSquirt::get_annoyed(int how_much) {}

void WaterSquirt::set_bribe() {}

///////////////////////////////////////////////////////////////////////////
///////////////////-----------WATER POOL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

WaterPool::WaterPool(int x, int y, StudentWorld* world)
:Actor(IID_WATER_POOL, x, y, right, 1.0, 2, world) {
  set_visible(true);
  m_ticks = max(100, 300 - 10 * returnWorld()->getCurrentLevel());
}

WaterPool::~WaterPool() {}

void WaterPool::do_something() {
  m_ticks--;
  if (!is_alive())
    return;
  //If ticks runs out, remove pool from oil field
  if (m_ticks <= 0) {
    set_alive(false);
    set_visible(false);
    return;
  }
  //Frackman picks up the pool of water
  if (returnWorld()->is_near_Frackman(this, 3)) {
    set_alive(false);
    returnWorld()->play_sound(SOUND_GOT_GOODIE);
    returnWorld()->set_squirts(5);
    returnWorld()->increase_score(100);
  }
}

void WaterPool::get_annoyed(int how_much) {}

void WaterPool::set_bribe() {}

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------DIRT--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

Dirt::Dirt(int x, int y, StudentWorld* world)
:Actor(IID_DIRT, x, y, right, 0.25, 3, world) { set_visible(true); }

Dirt::~Dirt() {}

void Dirt::do_something() {}

void Dirt::get_annoyed(int how_much) {}

bool Dirt::canActorsPassThroughMe() const { return false; }

void Dirt::set_bribe() {}
