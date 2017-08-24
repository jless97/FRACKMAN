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
/////////////////////-----------GLOBALS--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------ACTOR--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

Actor::Actor(int image_id, int start_x, int start_y, Direction start_dir, double image_size,
             int image_depth, StudentWorld* current_world)
: GraphObject(image_id, start_x, start_y, start_dir, image_size, image_depth), m_world(current_world), m_alive(true) {}

bool Actor::is_alive(void) { return m_alive; }

void Actor::set_dead(void) { m_alive = false; set_visible(false); }

StudentWorld* Actor::world(void) { return m_world; }

Actor::~Actor() {}

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------DIRT--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

Dirt::Dirt(int start_x, int start_y, StudentWorld* world)
: Actor(IID_DIRT, start_x, start_y, GraphObject::right, 0.25, 3, world) { set_visible(true); }

void Dirt::do_something() { return; }

Dirt::~Dirt() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------BOULDER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Boulder::Boulder(int start_x, int start_y, StudentWorld* world)
: Actor(IID_BOULDER, start_x, start_y, GraphObject::down, 1.00, 1, world), m_nticks_before_fall(30), m_state(stable)
{ set_visible(true); }

void Boulder::do_something(void) {
  // Check if Boulder is still alive
  if (!is_alive()) { set_dead(); }
  
  // Get current boulder coordinates
  int x = get_x();
  int y = get_y();
  
  // Get pointer to the StudentWorld
  StudentWorld* boulder_world = world();
  
  // Check if in dirt, if so delete the dirt
  boulder_world->remove_dirt(this);
  
  // Check state of the boulder
    // If in stable state, if there is no dirt directly below boulder, transition to waiting state
  if (m_state == 0) {
    if (!boulder_world->is_dirt_below(this)) { m_state = 1; }
  }
    // If in waiting state, if 30 ticks have elapsed, transition to falling state (and play sound effect for falling boulder)
  else if (m_state == 1) {
    if (m_nticks_before_fall-- <= 0) { m_state = 2; boulder_world->play_sound(SOUND_FALLING_ROCK); }
  }
    // If in falling state, then move down 1 step each tick, until it hits bottom of oil field, another boulder, dirt, or human
  else if (m_state == 2) {
    // If hits bottom of oil field
    if (y <= 0) { set_dead(); }
    // If hits another boulder
    else if (0) {} /// TODO: IMPLEMENT
    // If hits dirt
    else if (world()->is_dirt_below(this)) { set_dead(); }
    // If hits frackman
    else if (0) {}
    // If hits a protester
    else if (0) {}
    // Else move down one step
    else { move_to(x, y - 1); }
  }
  
  return;
}

Boulder::~Boulder() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
//////////////////-----------WATER SQUIRT--------------////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------HUMAN--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

Human::Human(int image_id, int start_x, int start_y, Direction start_dir, double image_size,
             int image_depth, StudentWorld* current_world, int start_health)
: Actor(image_id, start_x, start_y, start_dir, image_size, image_depth, current_world), m_health(start_health) {}

void Human::get_annoyed(int how_much) { m_health -= how_much; }

int Human::get_health(void) { return m_health; }

Human::~Human() {}

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------FRACKMAN--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Frackman::Frackman(StudentWorld* world)
: Human(IID_PLAYER, 30, 60, GraphObject::right, 1.00, 0, world, 10), m_squirts(5), m_sonars(1), m_gold(0) { set_visible(true); }

void Frackman::do_something() {
  // Check if Frackman is still alive
  if (!is_alive()) { return; }
  if (get_health() <= 0) { set_dead(); }
  
  // Get frackman's coordinates
  int x = get_x();
  int y = get_y();
  
  // Get pointer to the StudentWorld
  StudentWorld* frack_world = world();
  
  // Check if in dirt, if so delete the dirt (and play the digging sound effect)
  if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
  
  // Check if player pressed a valid key
  int key;
  if(frack_world->get_key(key)) {
    switch(key) {
      // Aborts current level (i.e. player lost a live, reset level (if lives remaining))
      case KEY_PRESS_ESCAPE:
        get_annoyed(10);
        break;
      // Frackman fires a water squirt from squirt gun
      case KEY_PRESS_SPACE:
        /// TODO: Implement
        break;
      // Left
      case KEY_PRESS_LEFT:
        // If not currently facing in the direction of key press
        if (get_direction() != GraphObject::left) { set_direction(GraphObject::left); break; }
        // If facing in the direction of key press, check that the movement is valid
        if (x <= 0) {
          // Move in place (per the spec)
          move_to(x, y);
        }
        else {
          // Check if there is a boulder blocking the way
          if (world()->radius_from_boulder(x - 2, y)) { move_to(x, y); }
          else {
            move_to(x - 1, y);
            if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
          }
        }
        break;
      // Right
      case KEY_PRESS_RIGHT:
        // If not currently facing in the direction of key press
        if (get_direction() != GraphObject::right) { set_direction(GraphObject::right); break; }
        // If facing in the direction of key press, check that the movement is valid
        if (x >= 60) {
          // Move in place (per the spec)
          move_to(x, y);
        }
        else {
          // Check if there is a boulder blocking the way
          if (world()->radius_from_boulder(x + 2, y)) { move_to(x, y); }
          else {
            move_to(x + 1, y);
            if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
          }
        }
        break;
      // Down
      case KEY_PRESS_DOWN:
        // If not currently facing in the direction of key press
        if (get_direction() != GraphObject::down) { set_direction(GraphObject::down); break; }
        // If facing in the direction of key press, check that the movement is valid
        if (y <= 0) {
          // Move in place (per the spec)
          move_to(x, y);
        }
        else {
          // Check if there is a boulder blocking the way
          if (world()->radius_from_boulder(x, y - 2)) { move_to(x, y); }
          else {
            move_to(x, y - 1);
            if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
          }
        }
        break;
      // Up
      case KEY_PRESS_UP:
        // If not currently facing in the direction of key press
        if (get_direction() != GraphObject::up) { set_direction(GraphObject::up); break; }
        // If facing in the direction of key press, check that the movement is valid
        if (y >= 60) {
          // Move in place (per the spec)
          move_to(x, y);
        }
        else {
          // Check if there is a boulder blocking the way
          if (world()->radius_from_boulder(x, y + 2)) { move_to(x, y); }
          else {
            move_to(x, y + 1);
            if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
          }
        }
        break;
      default:
        break;
    }
  }
  
  return;
}

int Frackman::get_squirts(void) { return m_squirts; }

int Frackman::get_gold(void) { return m_gold; }

int Frackman::get_sonars(void) { return m_sonars; }

Frackman::~Frackman() {}

///////////////////////////////////////////////////////////////////////////
////////////////////-----------PROTESTER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
////////////////-----------REGULAR PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////-----------HARDCORE PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOODIE--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------BARREL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOLD--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
////////////////////////-----------BRIBE--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
////////////////////-----------SONAR KIT--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////////-----------WATER POOL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////


