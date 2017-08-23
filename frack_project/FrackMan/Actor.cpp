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

Actor::Actor(int image_id, int start_x, int start_y, Direction start_direction,
             float size, unsigned int depth, StudentWorld* world)
:GraphObject(image_id, start_x, start_y, start_direction, size, depth),
m_world(world), m_is_alive(true)
{
  set_visible(false);
}

Actor::~Actor()
{
  m_is_alive = false;
  set_visible(false);
}

StudentWorld* Actor::return_world() const
{
  return m_world;
}

bool Actor::is_alive() const
{
  if (m_is_alive == true)
    return true;
  return false;
  
}

void Actor::set_alive(bool value)
{
  m_is_alive = false;
}

bool Actor::can_actors_pass_through_me() const
{
  return false;
}

bool Actor::move_dir(int direction, int num_moves)
{
  int x = get_x();
  int y = get_y();
  //Check to see if I'm allowed to move in a given direction
  if (return_world()->is_boulder(x, y, direction))
    return false;
  if (return_world()->is_dirt(x, y, direction))
    return false;
  if (!return_world()->can_actor_move_to(this, x, y))
    return false;
  
  //If so, then move in the specified direction
  switch (direction)
  {
    case GraphObject::up:
      move_to(x, y + num_moves);
      break;
    case GraphObject::down:
      move_to(x, y - num_moves);
      break;
    case GraphObject::right:
      move_to(x + num_moves, y);
      break;
    case GraphObject::left:
      move_to(x - num_moves, y);
      break;
    default:
      break;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////
/////////////////////-----------FRACKMAN--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

FrackMan::FrackMan(int x, int y, StudentWorld* world)
:Actor(IID_PLAYER, x, y, right, 1.0, 0, world), m_squirts(5), m_sonars(1), m_gold(0), m_health(10)
{
  set_visible(true);
}

FrackMan::~FrackMan()
{}

void FrackMan::do_something()
{
  if (!is_alive()) //Check to see if FrackMan is alive
    return;
  int x = get_x();
  int y = get_y();
  return_world()->remove_dirt(x, y); //If FrackMan spawns in a dirt location, remove the dirt
  int ch;
  StudentWorld* world = return_world();
  if (valid_position(x, y) == false)
    return;
  if (return_world()->get_key(ch) == true) //Check to see if a valid key was input by the user
  {
    switch (ch) //To see what FrackMan is going to do in the given turn (tick)
    {
      case KEY_PRESS_LEFT: //Direction checks to see if FrackMan can move in given direction
        if (get_direction() == left)
        {
          if (x != 0)
          {
            if (!world->is_near_boulder(x - 1, y, 3))
            {
              move_to(x - 1, y);
              if (world->remove_dirt(x - 1, y, KEY_PRESS_LEFT))
                world->play_sound(SOUND_DIG);
            }
            else if (world->can_actor_move_to(this, x - 1, y))
              set_direction(left);
          }
          if (x == 0)
            move_to(x, y);
        }
        else
          set_direction(left);
        break;
      case KEY_PRESS_RIGHT:
        if (get_direction() == right)
        {
          if (x != 60)
          {
            if (!world->is_near_boulder(x + 1, y, 3))
            {
              move_to(x + 1, y);
              if (world->remove_dirt(x + 1, y, KEY_PRESS_RIGHT))
                world->play_sound(SOUND_DIG);
            }
            else if (world->can_actor_move_to(this, x + 1, y))
              set_direction(right);
          }
          if (x == 60)
            move_to(x, y);
        }
        else
          set_direction(right);
        break;
      case KEY_PRESS_DOWN:
        if (get_direction() == down)
        {
          if (y != 0)
          {
            if (!world->is_near_boulder(x, y - 1, 3))
            {
              move_to(x, y - 1);
              if (world->remove_dirt(x, y - 1, KEY_PRESS_DOWN))
                world->play_sound(SOUND_DIG);
            }
            else if (world->can_actor_move_to(this, x, y - 1))
              set_direction(down);
          }
          if (y == 0)
            move_to(x, y);
        }
        else
          set_direction(down);
        break;
      case KEY_PRESS_UP:
        if (get_direction() == up)
        {
          if (y != 60)
          {
            if (!world->is_near_boulder(x, y + 1, 3))
            {
              move_to(x, y + 1);
              if (world->remove_dirt(x, y + 1, KEY_PRESS_UP))
                world->play_sound(SOUND_DIG);
            }
            else if (world->can_actor_move_to(this, x, y + 1))
              set_direction(up);
          }
          if (y == 60)
            move_to(x, y);
        }
        else
          set_direction(up);
        break;
      case KEY_PRESS_SPACE: //Shoots a squirt object (by creating a squirt object in oil field)
        if (m_squirts >= 1)
        {
          m_squirts--;
          return_world()->add_water_squirt(this, get_direction());
          return_world()->play_sound(SOUND_PLAYER_SQUIRT);
        }
        break;
      case KEY_PRESS_TAB: //Drops gold (by creating a bribe object in oil field)
        if (m_gold >= 1)
        {
          m_gold--;
          return_world()->add_bribe(this);
        }
        break;
      case KEY_PRESS_ESCAPE: //resets the level (or if no lives left, ends game)
        set_dead();
        break;
      case 'Z': //Uses a sonar to check for goodies in surrounding region
      case 'z':
        if (m_sonars >= 1)
        {
          m_sonars--;
          return_world()->is_near_actor(this, 12);
          return_world()->play_sound(SOUND_SONAR);
        }
        break;
      default:
        break;
    }
  }
}

void FrackMan::get_annoyed(int how_much)
{
  m_health -= 2;
  if (m_health <= 0)
  {
    set_dead();
    return_world()->play_sound(SOUND_PLAYER_GIVE_UP);
  }
}

bool FrackMan::valid_position(int x, int y) const
{
  if (x < 0 || x > 60 || y < 0 || y > 60)
    return false;
  return true;
}

int FrackMan::get_squirts() const
{
  return m_squirts;
}

int FrackMan::get_sonars() const
{
  return m_sonars;
}

int FrackMan::get_gold() const
{
  return m_gold;
}

int FrackMan::get_health() const
{
  return m_health;
}

void FrackMan::set_squirts(int how_much)
{
  m_squirts += how_much;
}

void FrackMan::set_sonars(int how_much)
{
  m_sonars += how_much;
}

void FrackMan::set_gold(int how_much)
{
  m_gold += how_much;
}

void FrackMan::set_dead()
{
  return_world()->play_sound(SOUND_PLAYER_GIVE_UP);
  set_visible(false);
  m_health = 0;
  set_alive(false);
}

void FrackMan::set_bribe()
{}



///////////////////////////////////////////////////////////////////////////
////////////////////-----------PROTESTER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Protester::Protester(int image_id, int start_x, int start_y, Direction start_direction, float size,
                     unsigned int depth, StudentWorld* world, int health)
:Actor(image_id, start_x, start_y, start_direction, size, depth, world), m_health(health)
{
  num_moves = world->rand_int(8, 60);
  resting_ticks = max(0, 3 - world->get_current_level() / 4);
  waiting_ticks_to_shout = 15;
  leave_field = false;
  set_visible(true);
}

Protester::~Protester()
{}

int Protester::get_health() const
{
  return m_health;
}

bool Protester::get_leave_oil_field_state() const
{
  return leave_field == true;
}


void Protester::dec_health(int hitpoints) {
  m_health -= hitpoints;
}

void Protester::set_dead()
{
  if (getID() == IID_PLAYER)
    return_world()->play_sound(SOUND_PLAYER_GIVE_UP);
  else if (getID() == IID_HARD_CORE_PROTESTER || getID() == IID_PROTESTER)
    return_world()->play_sound(SOUND_PROTESTER_GIVE_UP);
  
  set_visible(false);
  m_health = 0;
  set_alive(false);
}

void Protester::set_bribe()
{
  leave_field = true;
}

void Protester::set_leave_oil_field_state(bool value)
{
  leave_field = value;
}

void Protester::does_the_something()
{
  int x = get_x();
  int y = get_y();
  
  if (!is_alive())
    return;
  if (leave_field == true)
    resting_ticks = 0;
  
  if (resting_ticks > 0 && new_resting_ticks > 0)	// rest state
  {
    resting_ticks--;
    new_resting_ticks--;
    return;
  }
  if (resting_ticks > 0)
  {
    resting_ticks--;
    return;
  }
  if (new_resting_ticks > 0)
  {
    new_resting_ticks--;
    return;
  }
  if (resting_ticks == 0)
    resting_ticks = max(0, 3 - return_world()->get_current_level() / 4);
  
  waiting_ticks_to_shout--;
  
  if (leave_field == true)
  {
    if (get_x() == 60 && get_y() == 60) { // exit point
      set_dead();
      resting_ticks = INT_MAX;
      return;
    }
    // move one square closer to its exit point(at x = 60, y = 60)
    set_direction(up);
    if (return_world()->can_actor_move_to(this, get_x(), get_y()))
      do_move(x, y, get_direction());
    else {
      set_direction(right);
      do_move(x, y, get_direction());
    }
    
    resting_ticks = max(0, 3 - return_world()->get_current_level() / 4);
    return;
  }
  
  //If the protestor is within the proper distance of the frackman, then annoy him
  //Reset the ticks to wait before you can shout again, and reset resting ticks
  if (return_world()->is_near_frackman(this, 4)
      && return_world()->facing_toward_frackman(this)
      && waiting_ticks_to_shout <= 0)
  {
    return_world()->play_sound(SOUND_PROTESTER_YELL);
    return_world()->annoy_frackman();
    waiting_ticks_to_shout = 15;
    new_resting_ticks = max(50, 100 - return_world()->get_current_level() * 10);
    return;
  }
  
  //If the protester is right next to the frackman, then he should stop moving
  if (return_world()->is_near_frackman(this, 3))
    return;
  
  /// if frackman is in line of sight, move 1 toward
  
  GraphObject::Direction dir = return_world()->line_of_sight_to_frackman(this);
  
  if (dir != GraphObject::none && !return_world()->is_near_frackman(this, 3) )
  {
    // face frackman and take a step towards him if possible
    
    if (can_move(x, y, dir))
    {
      set_direction(dir);
      do_move(x, y, dir);
      num_moves = 0;
      return;
    }
  }
  
  if (num_moves > 0 && can_move(x, y, get_direction()))
  {
    do_move(x, y, get_direction());
    return;
  }
  else
  {
    //If number of moves is 0, and you can't move in a given direction, change direction
    change_direction_to_move();
  }
  
  return;
  
}

void Protester::get_annoyed(int how_much)
{
  dec_health(how_much);
  
  //If dead, then prepare to leave the oil field
  if (get_health() <= 0)
  {
    leave_field = true;
    return_world()->play_sound(SOUND_PROTESTER_GIVE_UP);
    resting_ticks = 0;
    return_world()->increase_score(100);
  }
  else //Just annoy the protester
  {
    return_world()->play_sound(SOUND_PROTESTER_ANNOYED);
    resting_ticks = max(50, 100 - return_world()->get_current_level() * 10);
  }
}

void Protester::change_direction_to_move()
{
  int x = get_x();
  int y = get_y();
  num_moves = 0;
  Direction direction = get_direction();
  do
  {
    int randNum = rand()%4;
    if (randNum == 0)
      direction = left;
    if (randNum == 1)
      direction = right;
    if (randNum == 2)
      direction = up;
    if (randNum == 3)
      direction = down;
  } while (can_move(x,y,direction) == false); //keep changind directions, until a valid direction is found
  set_direction(direction);
  num_moves = return_world()->rand_int(8, 60); //reset the number of moves
}

bool Protester::can_move(int x, int y, int direction)
{
  //Check to see if the protester can move in a given direction
  if (direction == left)
    if (return_world()->can_actor_move_to(this, x - 1, y))
      return true;
  if (direction == right)
    if (return_world()->can_actor_move_to(this, x + 1, y))
      return true;
  if (direction == up)
    if (return_world()->can_actor_move_to(this, x, y + 1))
      return true;
  if (direction == down)
    if (return_world()->can_actor_move_to(this, x, y - 1))
      return true;
  return false;
}

void Protester::do_move(int x, int y, int direction)
{
  //If you can move in a given direction, then move in that direction
  if (direction == left)
    if (can_move(x - 1, y, left))
      move_to(x - 1, y);
  if (direction == right)
    if (can_move(x - 1, y, right))
      move_to(x + 1, y);
  if (direction == up)
    if (can_move(x, y + 1, up))
      move_to(x, y + 1);
  if (direction == down)
    if (can_move(x, y - 1, down))
      move_to(x, y - 1);
}

///////////////////////////////////////////////////////////////////////////
////////////////-----------REGULAR PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

RegularProtester::RegularProtester(int x, int y, StudentWorld* world)
:Protester(IID_PROTESTER, x, y, left, 1.0, 0, world, 5)
{}

RegularProtester::~RegularProtester()
{}

void RegularProtester::do_something()
{
  //Call protester's method to initiate the move
  does_the_something();
}


///////////////////////////////////////////////////////////////////////////
///////////////-----------HARDCORE PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

HardcoreProtester::HardcoreProtester(int x, int y, StudentWorld* world)
:Protester(IID_HARD_CORE_PROTESTER, x, y, left, 1.0, 0, world, 20)
{}

HardcoreProtester::~HardcoreProtester()
{}

void HardcoreProtester::do_something()
{
  //Call the protester's method to intiate the move
  does_the_something();
}

void HardcoreProtester::set_bribe()
{
  set_leave_oil_field_state(false);
}

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------BOULDER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Boulder::Boulder(int x, int y, StudentWorld* world)
:Actor(IID_BOULDER, x, y, down, 1.0, 1, world), m_state(STABLE_STATE), m_waiting_state_counter(0)
{
  set_visible(true);
}

Boulder::~Boulder()
{}

int Boulder::get_waiting_state_counter() const
{
  return m_waiting_state_counter;
}

bool Boulder::is_dirt_below() const
{
  //Check to see if there is dirt below the boulder
  int x = get_x();
  int y = get_y();
  bool dirtBelow = false;
  for (int i = 0; i < 4; i++)
  {
    if (y != 0 && return_world()->is_dirt(x + i, y - 1))
      dirtBelow = true;
  }
  if (!dirtBelow)
  {
    return false;
  }
  return true;
}

void Boulder::move_boulder()
{
  bool frackManBelow = false;
  bool ProtesterBelow = false;
  bool boulderBelow = false;
  
  int x = get_x();
  int y = get_y();
  
  move_to(x, y - 1);
  
  //Check to see if the frackman, protester, or another boulder is below the boulder
  if (return_world()->is_near_frackman(this, 3))
    frackManBelow = true;
  if (is_dirt_below() || boulderBelow == true || y == -1)
  {
    set_alive(false);
  }
  if (ProtesterBelow == true)
  {
    return_world()->annoy_protester(this, 20);
    return_world()->increase_score(500);
  }
  if (frackManBelow == true)
  {
    return_world()->kill_frackman();
  }
}

void Boulder::do_something()
{
  if (!is_alive())
    return;
  
  //Initial state when there is dirt below the boulder
  if (m_state == STABLE_STATE)
  {
    if (!is_dirt_below())
    {
      m_state = WAITING_STATE;
      m_waiting_state_counter++;
    }
    else
      return;
  }
  if (m_state == WAITING_STATE) //If dirt was removed below the boulder, change to waiting state
  {
    if (get_waiting_state_counter() == 30)
    {
      m_state = FALLING_STATE;
      return_world()->play_sound(SOUND_FALLING_ROCK);
    }
    else
      m_waiting_state_counter++;
  }
  if (m_state == FALLING_STATE) //If the boulder is in a falling state check to see if it can move down
  {
    move_boulder();
  }
}

void Boulder::get_annoyed(int how_much)
{}

bool Boulder::can_actors_pass_through_me() const
{
  return false;
}

void Boulder::set_bribe()
{}


///////////////////////////////////////////////////////////////////////////
///////////////////////-----------BARREL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Barrel::Barrel(int x, int y, StudentWorld* world)
:Actor(IID_BARREL, x, y, right, 1.0, 2, world)
{}

Barrel::~Barrel()
{}

void Barrel::do_something()
{
  if (!is_alive())
    return;
  
  if (!is_visible() && return_world()->is_near_frackman(this, 4)) //Activate visibility
  {
    set_visible(true);
    return;
  }
  if (return_world()->is_near_frackman(this, 3)) //Frackman picks up barrel
  {
    set_alive(false);
    return_world()->set_barrel(-1);
    return_world()->play_sound(SOUND_FOUND_OIL);
    return_world()->increase_score(1000);
    return;
  }
  
}

void Barrel::get_annoyed(int how_much)
{}

void Barrel::set_bribe()
{}


///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOLD--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

Gold::Gold(int x, int y, StudentWorld* world)
:Actor(IID_GOLD, x, y, right, 1.0, 2, world)
{}

Gold::~Gold()
{}

void Gold::do_something()
{
  if (!is_alive())
    return;
  if (!is_visible() && return_world()->is_near_frackman(this, 4)) //Activate visibility
  {
    set_visible(true);
    return;
  }
  if (return_world()->is_near_frackman(this, 3)) //FrackMan picks up gold
  {
    set_alive(false);
    set_visible(false);
    return_world()->play_sound(SOUND_GOT_GOODIE);
    return_world()->increase_score(10);
    return_world()->set_gold(1);
  }
  
}

void Gold::get_annoyed(int how_much)
{}

void Gold::set_bribe()
{}


///////////////////////////////////////////////////////////////////////////
////////////////////////-----------BRIBE--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Bribe::Bribe(int x, int y, StudentWorld* world)
:Actor(IID_GOLD, x, y, right, 1.0, 2, world)
{
  set_visible(true);
  m_ticks = 100;
}

void Bribe::do_something()
{
  if (!is_alive())
    return;
  m_ticks--;
  if (m_ticks <= 0)
  {
    set_alive(false);
    set_visible(false);
    return;
  }
  if (return_world()->is_near_protester(this, 3)) //Protester picks up bribe
  {
    set_alive(false);
    return_world()->play_sound(SOUND_PROTESTER_FOUND_GOLD);
    return_world()->increase_score(25);
    return_world()->set_bribe(1);
  }
}

void Bribe::get_annoyed(int how_much)
{}

Bribe::~Bribe()
{}

void Bribe::set_bribe()
{}


///////////////////////////////////////////////////////////////////////////
////////////////////-----------SONAR KIT--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Sonar::Sonar(int x, int y, StudentWorld* world)
:Actor(IID_SONAR, x, y, right, 1.0, 2, world)
{
  set_visible(true);
  m_ticks = max(100, 300 - 10 * return_world()->get_current_level());
}

Sonar::~Sonar()
{}

void Sonar::do_something()
{
  if (!is_alive())
    return;
  m_ticks--;
  if (m_ticks <= 0) //If ticks expire, remove from oil field
  {
    set_alive(false);
    set_visible(false);
    return;
  }
  if (return_world()->is_near_frackman(this, 3)) //FrackMan picks up sonar
  {
    set_alive(false);
    return_world()->play_sound(SOUND_GOT_GOODIE);
    return_world()->set_sonars(1);
    return_world()->increase_score(75);
  }
}

void Sonar::get_annoyed(int how_much)
{}

void Sonar::set_bribe()
{}


///////////////////////////////////////////////////////////////////////////
//////////////////-----------WATER SQUIRT--------------////////////////////
///////////////////////////////////////////////////////////////////////////

WaterSquirt::WaterSquirt(int x, int y, Direction direction, StudentWorld* world)
:Actor(IID_WATER_SPURT, x, y, direction, 1.0, 1, world), m_distance(5)
{
  set_visible(true);
}

WaterSquirt::~WaterSquirt()
{}

void WaterSquirt::do_something()
{
  int direction = get_direction();
  StudentWorld* world = return_world();
  int x = get_x();
  int y = get_y();
  bool is_dirt = false;
  
  if (!is_alive())
    return;
  
  if (return_world()->is_near_protester(this, 3)) //If next to protester, stop moving
  {
    return_world()->annoy_protester(this, 2);
    set_alive(false);
    set_visible(false);
  }
  
  if (m_distance == 0) //If squirt traveled its distance, then remove squirt
  {
    set_alive(false);
    set_visible(false);
  }
  
  switch (direction) //Check to see if squirt can move in the given direction
  {
    case left:
      if (world->is_dirt(x, y, left) || (x < 0))
        is_dirt = true;
      break;
    case right:
      if (world->is_dirt(x, y, right) || (x > 60))
        is_dirt = true;
      break;
    case down:
      if (world->is_dirt(x, y, down) || (y < 0))
        is_dirt = true;
      break;
    case up:
      if (world->is_dirt(x, y, up) || (y > 60))
        is_dirt = true;
      break;
  }
  if (is_dirt == true ||
      return_world()->is_near_boulder(get_x(), get_y(), 3))
  {
    set_alive(false);
    set_visible(false);
  }
  
  else
  {
    move_dir(get_direction(), 1); //If the squirt moves, then reduce it's allowed travel distance
    m_distance--;
  }
  
}

void WaterSquirt::get_annoyed(int how_much)
{}

void WaterSquirt::set_bribe()
{}


///////////////////////////////////////////////////////////////////////////
///////////////////-----------WATER POOL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

WaterPool::WaterPool(int x, int y, StudentWorld* world)
:Actor(IID_WATER_POOL, x, y, right, 1.0, 2, world)
{
  set_visible(true);
  m_ticks = max(100, 300 - 10 * return_world()->get_current_level());
}

WaterPool::~WaterPool()
{}

void WaterPool::do_something()
{
  m_ticks--;
  if (!is_alive())
    return;
  if (m_ticks <= 0) //If ticks runs out, remove pool from oil field
  {
    set_alive(false);
    set_visible(false);
    return;
  }
  if (return_world()->is_near_frackman(this, 3)) //FrackMan picks up the pool of water
  {
    set_alive(false);
    return_world()->play_sound(SOUND_GOT_GOODIE);
    return_world()->set_squirts(5);
    return_world()->increase_score(100);
  }
}

void WaterPool::get_annoyed(int how_much)
{}

void WaterPool::set_bribe()
{}


///////////////////////////////////////////////////////////////////////////
///////////////////////-----------DIRT--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

Dirt::Dirt(int x, int y, StudentWorld* world)
:Actor(IID_DIRT, x, y, right, 0.25, 3, world)
{
  set_visible(true);
}

Dirt::~Dirt()
{}

void Dirt::do_something()
{}

void Dirt::get_annoyed(int how_much)
{}

bool Dirt::can_actors_pass_through_me() const
{
  return false;
}

void Dirt::set_bribe()
{}
