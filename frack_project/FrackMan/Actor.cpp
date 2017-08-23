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
Actor::Actor(int imageID, int startX, int startY, Direction startDirection,
             float size, UINT depth,StudentWorld* world)
:GraphObject(imageID, startX, startY, startDirection, size, depth), m_world(world) {
  setVisible(false);
}

Actor::~Actor() {
  setVisible(false);
}

StudentWorld* Actor::returnWorld() const {
  return m_world;
}

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------FRACKMAN--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

FrackMan::FrackMan(StudentWorld* world)
:Actor(IID_PLAYER, 30, 60, right, 1.0, 0, world), m_health(10), m_squirts(5), m_sonars(1), m_gold(0) {
  setVisible(true);
}

FrackMan::~FrackMan() {
}

void FrackMan::doSomething() {
  //Check to see if FrackMan is alive
  if (!isAlive()) {
        return;
  }
  
  int x = getX();
  int y = getY();
  
  //If FrackMan spawns in a dirt location, remove the dirt
  returnWorld()->removeDirt(x,y,none);
  int ch;
  //Check to see if a valid key was input by the user
  if (returnWorld()->getKey(ch) == true) {
    //To see what FrackMan is going to do in the given turn (tick)
    switch (ch) {
      case KEY_PRESS_LEFT:
        if (getDirection() == left) {
          moveFrackMan(ch);
        }
        else {
          setDirection(left);
        }
        break;
      case KEY_PRESS_RIGHT:
        if (getDirection() == right) {
          moveFrackMan(ch);
        }
        else {
          setDirection(right);
        }
        break;
      case KEY_PRESS_UP:
        if (getDirection() == up) {
          moveFrackMan(ch);
        }
        else {
          setDirection(up);
        }
        break;
      case KEY_PRESS_DOWN:
        if (getDirection() == down) {
          moveFrackMan(ch);
        }
        else {
          setDirection(down);
        }
        break;
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
  int x = getX();
  int y = getY();
  
  if (validPosition(x, y) == false) {
    return;
  }
  //To see what direction to move FrackMan, and if the SOUND_DIG should play
  switch (dir) {
    case KEY_PRESS_LEFT:
      if (x != 0) {
        //If dirt was removed, play sound
        if (returnWorld()->removeDirt(x - 1, y, KEY_PRESS_LEFT)) {
            returnWorld()->playSound(SOUND_DIG);
        }
        moveTo(x - 1, y);
      }
      if (x == 0) {
        moveTo(x,y);
      }
      break;
    case KEY_PRESS_RIGHT:
      if (x != 60) {
        if (returnWorld()->removeDirt(x + 1, y, KEY_PRESS_RIGHT)) {
          returnWorld()->playSound(SOUND_DIG);
        }
        moveTo(x + 1, y);
      }
      if (x == 60) {
        moveTo(x,y);
      }
      break;
    case KEY_PRESS_DOWN:
      if (y != 0) {
        if (returnWorld()->removeDirt(x, y - 1, KEY_PRESS_DOWN)) {
          returnWorld()->playSound(SOUND_DIG);
        }
        moveTo(x, y - 1);
      }
      if (y == 0) {
        moveTo(x,y);
      }
      break;
    case KEY_PRESS_UP:
      if (y != 60) {
        if (returnWorld()->removeDirt(x, y + 1, KEY_PRESS_UP)) {
          returnWorld()->playSound(SOUND_DIG);
        }
        moveTo(x, y + 1);
      }
      if (y == 60) {
        moveTo(x,y);
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
  setVisible(true);
}

Dirt::~Dirt() {
}

void Dirt::doSomething() {
}
