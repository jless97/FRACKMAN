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
using namespace std;

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------ACTOR--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

Actor::Actor(int image_id, int start_x, int start_y, Direction start_direction,
             float size, UINT depth, StudentWorld* world)
:GraphObject(image_id, start_x, start_y, start_direction, size, depth), m_world(world) {
}

Actor::~Actor() {
  set_visible(false);
}

StudentWorld* Actor::return_world() const {
  return m_world;
}

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------FRACKMAN--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

FrackMan::FrackMan(StudentWorld* world)
:Actor(IID_PLAYER, 30, 60, right, 1.0, 0, world), m_health(10), m_squirts(5), m_sonars(1), m_gold(0) {
  set_visible(true);
}

FrackMan::~FrackMan() {
}

void FrackMan::do_something() {
  //Check to see if FrackMan is alive
  if (!isAlive()) {
    return;
  }

  //Check to see if a valid key was input by the user
  int direction;
  if (return_world()->getKey(direction)) {
    // Update FrackMan depending on the button pressed
    /* Note: When a directional button is pressed:
     (1) If facing the direction of the button pressed, then move one step in that direction
     (2) If not facing in that direction, then change FrackMan's direction
     */
    switch (direction) {
      case KEY_PRESS_LEFT:
        if (get_direction() == left) {
          moveFrackMan(direction);
        }
        else {
          set_direction(left);
        }
        break;
      case KEY_PRESS_RIGHT:
        if (get_direction() == right) {
          moveFrackMan(direction);
        }
        else {
          set_direction(right);
        }
        break;
      case KEY_PRESS_UP:
        if (get_direction() == up) {
          moveFrackMan(direction);
        }
        else {
          set_direction(up);
        }
        break;
      case KEY_PRESS_DOWN:
        if (get_direction() == down) {
          moveFrackMan(direction);
        }
        else {
          set_direction(down);
        }
        break;
      // TODO: implement (non-directional button presses)
      case KEY_PRESS_SPACE:
        
        break;
      case KEY_PRESS_TAB:
          
        break;
      case KEY_PRESS_ESCAPE:
        break;
      case 'Z':
      case 'z':
        break;
      default:
        break;
    }
  }
}

bool FrackMan::isAlive() const {
  return (m_health > 0);
}

bool FrackMan::validPosition(int x, int y) const {
  if (x < 0 || x > 60 || y < 0 || y > 60) {
      return false;
  }
  
  return true;
}

void FrackMan::moveFrackMan(int dir) {
  // Get FrackMan's current coordinates
  int x = get_x();
  int y = get_y();
  
  if (validPosition(x, y) == false) {
    return;
  }
  
  // Update FrackMan's location (and remove dirt if in his path)
  // Note: For edges of the oil field, FrackMan movement animation should still occur
  switch (dir) {
    case KEY_PRESS_LEFT:
      if (x != EDGE_LOWER) {
        //If dirt was removed, play sound
//        if (return_world()->removeDirt(x - 1, y, KEY_PRESS_LEFT)) {
//            return_world()->playSound(SOUND_DIG);
//        }
        move_to(x - 1, y);
      }
      if (x == EDGE_LOWER) {
        move_to(x,y);
      }
      break;
    case KEY_PRESS_RIGHT:
      if (x != EDGE_UPPER) {
//        if (return_world()->removeDirt(x + 1, y, KEY_PRESS_RIGHT)) {
//          return_world()->playSound(SOUND_DIG);
//        }
        move_to(x + 1, y);
      }
      if (x == EDGE_UPPER) {
        move_to(x,y);
      }
      break;
    case KEY_PRESS_DOWN:
      if (y != EDGE_LOWER) {
//        if (return_world()->removeDirt(x, y - 1, KEY_PRESS_DOWN)) {
//          return_world()->playSound(SOUND_DIG);
//        }
        move_to(x, y - 1);
      }
      if (y == EDGE_LOWER) {
        move_to(x,y);
      }
      break;
    case KEY_PRESS_UP:
      if (y != EDGE_UPPER) {
//        if (return_world()->removeDirt(x, y + 1, KEY_PRESS_UP)) {
//          return_world()->playSound(SOUND_DIG);
//        }
        move_to(x, y + 1);
      }
      if (y == EDGE_UPPER) {
        move_to(x,y);
      }
      break;
    default:
      break;
  }
    
}

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------DIRT--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

Dirt::Dirt(StudentWorld* world, int x, int y)
:Actor(IID_DIRT, x, y, right, 0.25, 3, world) {
  set_visible(true);
}

Dirt::~Dirt() {
}

void Dirt::do_something() {
}
