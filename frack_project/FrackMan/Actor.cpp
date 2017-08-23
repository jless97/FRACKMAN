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

Actor::Actor(int imageID, int startX, int startY, Direction startDirection,
             float size, unsigned int depth, StudentWorld* world)
:GraphObject(imageID, startX, startY, startDirection, size, depth),
m_world(world), m_isAlive(true)
{
  set_visible(false);
}

Actor::~Actor()
{
  m_isAlive = false;
  set_visible(false);
}

StudentWorld* Actor::returnWorld() const
{
  return m_world;
}

bool Actor::isAlive() const
{
  if (m_isAlive == true)
    return true;
  return false;
  
}

void Actor::setAlive(bool value)
{
  m_isAlive = false;
}

bool Actor::canActorsPassThroughMe() const
{
  return false;
}

bool Actor::moveDir(int direction, int numMoves)
{
  int x = get_x();
  int y = get_y();
  //Check to see if I'm allowed to move in a given direction
  if (returnWorld()->isBoulder(x, y, direction))
    return false;
  if (returnWorld()->isDirt(x, y, direction))
    return false;
  if (!returnWorld()->canActormove_to(this, x, y))
    return false;
  
  //If so, then move in the specified direction
  switch (direction)
  {
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

FrackMan::FrackMan(int x, int y, StudentWorld* world)
:Actor(IID_PLAYER, x, y, right, 1.0, 0, world), m_squirts(5), m_sonars(1), m_gold(0), m_health(10)
{
  set_visible(true);
}

FrackMan::~FrackMan()
{}

void FrackMan::doSomething()
{
  if (!isAlive()) //Check to see if FrackMan is alive
    return;
  int x = get_x();
  int y = get_y();
  returnWorld()->removeDirt(x, y); //If FrackMan spawns in a dirt location, remove the dirt
  int ch;
  StudentWorld* world = returnWorld();
  if (validPosition(x, y) == false)
    return;
  if (returnWorld()->get_key(ch) == true) //Check to see if a valid key was input by the user
  {
    switch (ch) //To see what FrackMan is going to do in the given turn (tick)
    {
      case KEY_PRESS_LEFT: //Direction checks to see if FrackMan can move in given direction
        if (get_direction() == left)
        {
          if (x != 0)
          {
            if (!world->isNearBoulder(x - 1, y, 3))
            {
              move_to(x - 1, y);
              if (world->removeDirt(x - 1, y, KEY_PRESS_LEFT))
                world->play_sound(SOUND_DIG);
            }
            else if (world->canActormove_to(this, x - 1, y))
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
            if (!world->isNearBoulder(x + 1, y, 3))
            {
              move_to(x + 1, y);
              if (world->removeDirt(x + 1, y, KEY_PRESS_RIGHT))
                world->play_sound(SOUND_DIG);
            }
            else if (world->canActormove_to(this, x + 1, y))
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
            if (!world->isNearBoulder(x, y - 1, 3))
            {
              move_to(x, y - 1);
              if (world->removeDirt(x, y - 1, KEY_PRESS_DOWN))
                world->play_sound(SOUND_DIG);
            }
            else if (world->canActormove_to(this, x, y - 1))
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
            if (!world->isNearBoulder(x, y + 1, 3))
            {
              move_to(x, y + 1);
              if (world->removeDirt(x, y + 1, KEY_PRESS_UP))
                world->play_sound(SOUND_DIG);
            }
            else if (world->canActormove_to(this, x, y + 1))
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
          returnWorld()->addWaterSquirt(this, get_direction());
          returnWorld()->play_sound(SOUND_PLAYER_SQUIRT);
        }
        break;
      case KEY_PRESS_TAB: //Drops gold (by creating a bribe object in oil field)
        if (m_gold >= 1)
        {
          m_gold--;
          returnWorld()->addBribe(this);
        }
        break;
      case KEY_PRESS_ESCAPE: //resets the level (or if no lives left, ends game)
        setDead();
        break;
      case 'Z': //Uses a sonar to check for goodies in surrounding region
      case 'z':
        if (m_sonars >= 1)
        {
          m_sonars--;
          returnWorld()->isNearActor(this, 12);
          returnWorld()->play_sound(SOUND_SONAR);
        }
        break;
      default:
        break;
    }
  }
}

void FrackMan::getAnnoyed(int howMuch)
{
  m_health -= 2;
  if (m_health <= 0)
  {
    setDead();
    returnWorld()->play_sound(SOUND_PLAYER_GIVE_UP);
  }
}

bool FrackMan::validPosition(int x, int y) const
{
  if (x < 0 || x > 60 || y < 0 || y > 60)
    return false;
  return true;
}

int FrackMan::getSquirts() const
{
  return m_squirts;
}

int FrackMan::getSonars() const
{
  return m_sonars;
}

int FrackMan::getGold() const
{
  return m_gold;
}

int FrackMan::getHealth() const
{
  return m_health;
}

void FrackMan::setSquirts(int howMuch)
{
  m_squirts += howMuch;
}

void FrackMan::setSonars(int howMuch)
{
  m_sonars += howMuch;
}

void FrackMan::setGold(int howMuch)
{
  m_gold += howMuch;
}

void FrackMan::setDead()
{
  returnWorld()->play_sound(SOUND_PLAYER_GIVE_UP);
  set_visible(false);
  m_health = 0;
  setAlive(false);
}

void FrackMan::setBribe()
{}



///////////////////////////////////////////////////////////////////////////
////////////////////-----------PROTESTER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Protester::Protester(int imageID, int startX, int startY, Direction startDirection, float size,
                     unsigned int depth, StudentWorld* world, int health)
:Actor(imageID, startX, startY, startDirection, size, depth, world), m_health(health)
{
  numMoves = world->randInt(8, 60);
  restingTicks = max(0, 3 - world->getCurrentLevel() / 4);
  waitingTicksToShout = 15;
  leaveField = false;
  set_visible(true);
}

Protester::~Protester()
{}

int Protester::getHealth() const
{
  return m_health;
}

bool Protester::getLeaveOilFieldState() const
{
  return leaveField == true;
}


void Protester::decHealth(int hitpoints) {
  m_health -= hitpoints;
}

void Protester::setDead()
{
  if (getID() == IID_PLAYER)
    returnWorld()->play_sound(SOUND_PLAYER_GIVE_UP);
  else if (getID() == IID_HARD_CORE_PROTESTER || getID() == IID_PROTESTER)
    returnWorld()->play_sound(SOUND_PROTESTER_GIVE_UP);
  
  set_visible(false);
  m_health = 0;
  setAlive(false);
}

void Protester::setBribe()
{
  leaveField = true;
}

void Protester::setLeaveOilFieldState(bool value)
{
  leaveField = value;
}

void Protester::doesTheSomething()
{
  int x = get_x();
  int y = get_y();
  
  if (!isAlive())
    return;
  if (leaveField == true)
    restingTicks = 0;
  
  if (restingTicks > 0 && newRestingTicks > 0)	// rest state
  {
    restingTicks--;
    newRestingTicks--;
    return;
  }
  if (restingTicks > 0)
  {
    restingTicks--;
    return;
  }
  if (newRestingTicks > 0)
  {
    newRestingTicks--;
    return;
  }
  if (restingTicks == 0)
    restingTicks = max(0, 3 - returnWorld()->getCurrentLevel() / 4);
  
  waitingTicksToShout--;
  
  if (leaveField == true)
  {
    if (get_x() == 60 && get_y() == 60) { // exit point
      setDead();
      restingTicks = INT_MAX;
      return;
    }
    // move one square closer to its exit point(at x = 60, y = 60)
    set_direction(up);
    if (returnWorld()->canActormove_to(this, get_x(), get_y()))
      doMove(x, y, get_direction());
    else {
      set_direction(right);
      doMove(x, y, get_direction());
    }
    
    restingTicks = max(0, 3 - returnWorld()->getCurrentLevel() / 4);
    return;
  }
  
  //If the protestor is within the proper distance of the frackman, then annoy him
  //Reset the ticks to wait before you can shout again, and reset resting ticks
  if (returnWorld()->isNearFrackMan(this, 4)
      && returnWorld()->facingTowardFrackMan(this)
      && waitingTicksToShout <= 0)
  {
    returnWorld()->play_sound(SOUND_PROTESTER_YELL);
    returnWorld()->annoyFrackMan();
    waitingTicksToShout = 15;
    newRestingTicks = max(50, 100 - returnWorld()->getCurrentLevel() * 10);
    return;
  }
  
  //If the protester is right next to the frackman, then he should stop moving
  if (returnWorld()->isNearFrackMan(this, 3))
    return;
  
  /// if frackman is in line of sight, move 1 toward
  
  GraphObject::Direction dir = returnWorld()->lineOfSightToFrackMan(this);
  
  if (dir != GraphObject::none && !returnWorld()->isNearFrackMan(this, 3) )
  {
    // face frackman and take a step towards him if possible
    
    if (canMove(x, y, dir))
    {
      set_direction(dir);
      doMove(x, y, dir);
      numMoves = 0;
      return;
    }
  }
  
  if (numMoves > 0 && canMove(x, y, get_direction()))
  {
    doMove(x, y, get_direction());
    return;
  }
  else
  {
    //If number of moves is 0, and you can't move in a given direction, change direction
    changeDirectionToMove();
  }
  
  return;
  
}

void Protester::getAnnoyed(int howMuch)
{
  decHealth(howMuch);
  
  //If dead, then prepare to leave the oil field
  if (getHealth() <= 0)
  {
    leaveField = true;
    returnWorld()->play_sound(SOUND_PROTESTER_GIVE_UP);
    restingTicks = 0;
    returnWorld()->increase_score(100);
  }
  else //Just annoy the protester
  {
    returnWorld()->play_sound(SOUND_PROTESTER_ANNOYED);
    restingTicks = max(50, 100 - returnWorld()->getCurrentLevel() * 10);
  }
}

void Protester::changeDirectionToMove()
{
  int x = get_x();
  int y = get_y();
  numMoves = 0;
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
  } while (canMove(x,y,direction) == false); //keep changind directions, until a valid direction is found
  set_direction(direction);
  numMoves = returnWorld()->randInt(8, 60); //reset the number of moves
}

bool Protester::canMove(int x, int y, int direction)
{
  //Check to see if the protester can move in a given direction
  if (direction == left)
    if (returnWorld()->canActormove_to(this, x - 1, y))
      return true;
  if (direction == right)
    if (returnWorld()->canActormove_to(this, x + 1, y))
      return true;
  if (direction == up)
    if (returnWorld()->canActormove_to(this, x, y + 1))
      return true;
  if (direction == down)
    if (returnWorld()->canActormove_to(this, x, y - 1))
      return true;
  return false;
}

void Protester::doMove(int x, int y, int direction)
{
  //If you can move in a given direction, then move in that direction
  if (direction == left)
    if (canMove(x - 1, y, left))
      move_to(x - 1, y);
  if (direction == right)
    if (canMove(x - 1, y, right))
      move_to(x + 1, y);
  if (direction == up)
    if (canMove(x, y + 1, up))
      move_to(x, y + 1);
  if (direction == down)
    if (canMove(x, y - 1, down))
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

void RegularProtester::doSomething()
{
  //Call protester's method to initiate the move
  doesTheSomething();
}


///////////////////////////////////////////////////////////////////////////
///////////////-----------HARDCORE PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

HardcoreProtester::HardcoreProtester(int x, int y, StudentWorld* world)
:Protester(IID_HARD_CORE_PROTESTER, x, y, left, 1.0, 0, world, 20)
{}

HardcoreProtester::~HardcoreProtester()
{}

void HardcoreProtester::doSomething()
{
  //Call the protester's method to intiate the move
  doesTheSomething();
}

void HardcoreProtester::setBribe()
{
  setLeaveOilFieldState(false);
}

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------BOULDER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Boulder::Boulder(int x, int y, StudentWorld* world)
:Actor(IID_BOULDER, x, y, down, 1.0, 1, world), m_state(STABLE_STATE), m_waitingStateCounter(0)
{
  set_visible(true);
}

Boulder::~Boulder()
{}

int Boulder::getWaitingStateCounter() const
{
  return m_waitingStateCounter;
}

bool Boulder::isDirtBelow() const
{
  //Check to see if there is dirt below the boulder
  int x = get_x();
  int y = get_y();
  bool dirtBelow = false;
  for (int i = 0; i < 4; i++)
  {
    if (y != 0 && returnWorld()->isDirt(x + i, y - 1))
      dirtBelow = true;
  }
  if (!dirtBelow)
  {
    return false;
  }
  return true;
}

void Boulder::moveBoulder()
{
  bool frackManBelow = false;
  bool ProtesterBelow = false;
  bool boulderBelow = false;
  
  int x = get_x();
  int y = get_y();
  
  move_to(x, y - 1);
  
  //Check to see if the frackman, protester, or another boulder is below the boulder
  if (returnWorld()->isNearFrackMan(this, 3))
    frackManBelow = true;
  if (isDirtBelow() || boulderBelow == true || y == -1)
  {
    setAlive(false);
  }
  if (ProtesterBelow == true)
  {
    returnWorld()->annoyProtester(this, 20);
    returnWorld()->increase_score(500);
  }
  if (frackManBelow == true)
  {
    returnWorld()->killFrackMan();
  }
}

void Boulder::doSomething()
{
  if (!isAlive())
    return;
  
  //Initial state when there is dirt below the boulder
  if (m_state == STABLE_STATE)
  {
    if (!isDirtBelow())
    {
      m_state = WAITING_STATE;
      m_waitingStateCounter++;
    }
    else
      return;
  }
  if (m_state == WAITING_STATE) //If dirt was removed below the boulder, change to waiting state
  {
    if (getWaitingStateCounter() == 30)
    {
      m_state = FALLING_STATE;
      returnWorld()->play_sound(SOUND_FALLING_ROCK);
    }
    else
      m_waitingStateCounter++;
  }
  if (m_state == FALLING_STATE) //If the boulder is in a falling state check to see if it can move down
  {
    moveBoulder();
  }
}

void Boulder::getAnnoyed(int howMuch)
{}

bool Boulder::canActorsPassThroughMe() const
{
  return false;
}

void Boulder::setBribe()
{}


///////////////////////////////////////////////////////////////////////////
///////////////////////-----------BARREL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Barrel::Barrel(int x, int y, StudentWorld* world)
:Actor(IID_BARREL, x, y, right, 1.0, 2, world)
{}

Barrel::~Barrel()
{}

void Barrel::doSomething()
{
  if (!isAlive())
    return;
  
  if (!is_visible() && returnWorld()->isNearFrackMan(this, 4)) //Activate visibility
  {
    set_visible(true);
    return;
  }
  if (returnWorld()->isNearFrackMan(this, 3)) //Frackman picks up barrel
  {
    setAlive(false);
    returnWorld()->setBarrel(-1);
    returnWorld()->play_sound(SOUND_FOUND_OIL);
    returnWorld()->increase_score(1000);
    return;
  }
  
}

void Barrel::getAnnoyed(int howMuch)
{}

void Barrel::setBribe()
{}


///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOLD--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

Gold::Gold(int x, int y, StudentWorld* world)
:Actor(IID_GOLD, x, y, right, 1.0, 2, world)
{}

Gold::~Gold()
{}

void Gold::doSomething()
{
  if (!isAlive())
    return;
  if (!is_visible() && returnWorld()->isNearFrackMan(this, 4)) //Activate visibility
  {
    set_visible(true);
    return;
  }
  if (returnWorld()->isNearFrackMan(this, 3)) //FrackMan picks up gold
  {
    setAlive(false);
    set_visible(false);
    returnWorld()->play_sound(SOUND_GOT_GOODIE);
    returnWorld()->increase_score(10);
    returnWorld()->setGold(1);
  }
  
}

void Gold::getAnnoyed(int howMuch)
{}

void Gold::setBribe()
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

void Bribe::doSomething()
{
  if (!isAlive())
    return;
  m_ticks--;
  if (m_ticks <= 0)
  {
    setAlive(false);
    set_visible(false);
    return;
  }
  if (returnWorld()->isNearProtester(this, 3)) //Protester picks up bribe
  {
    setAlive(false);
    returnWorld()->play_sound(SOUND_PROTESTER_FOUND_GOLD);
    returnWorld()->increase_score(25);
    returnWorld()->setBribe(1);
  }
}

void Bribe::getAnnoyed(int howMuch)
{}

Bribe::~Bribe()
{}

void Bribe::setBribe()
{}


///////////////////////////////////////////////////////////////////////////
////////////////////-----------SONAR KIT--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Sonar::Sonar(int x, int y, StudentWorld* world)
:Actor(IID_SONAR, x, y, right, 1.0, 2, world)
{
  set_visible(true);
  m_ticks = max(100, 300 - 10 * returnWorld()->getCurrentLevel());
}

Sonar::~Sonar()
{}

void Sonar::doSomething()
{
  if (!isAlive())
    return;
  m_ticks--;
  if (m_ticks <= 0) //If ticks expire, remove from oil field
  {
    setAlive(false);
    set_visible(false);
    return;
  }
  if (returnWorld()->isNearFrackMan(this, 3)) //FrackMan picks up sonar
  {
    setAlive(false);
    returnWorld()->play_sound(SOUND_GOT_GOODIE);
    returnWorld()->setSonars(1);
    returnWorld()->increase_score(75);
  }
}

void Sonar::getAnnoyed(int howMuch)
{}

void Sonar::setBribe()
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

void WaterSquirt::doSomething()
{
  int direction = get_direction();
  StudentWorld* world = returnWorld();
  int x = get_x();
  int y = get_y();
  bool isDirt = false;
  
  if (!isAlive())
    return;
  
  if (returnWorld()->isNearProtester(this, 3)) //If next to protester, stop moving
  {
    returnWorld()->annoyProtester(this, 2);
    setAlive(false);
    set_visible(false);
  }
  
  if (m_distance == 0) //If squirt traveled its distance, then remove squirt
  {
    setAlive(false);
    set_visible(false);
  }
  
  switch (direction) //Check to see if squirt can move in the given direction
  {
    case left:
      if (world->isDirt(x, y, left) || (x < 0))
        isDirt = true;
      break;
    case right:
      if (world->isDirt(x, y, right) || (x > 60))
        isDirt = true;
      break;
    case down:
      if (world->isDirt(x, y, down) || (y < 0))
        isDirt = true;
      break;
    case up:
      if (world->isDirt(x, y, up) || (y > 60))
        isDirt = true;
      break;
  }
  if (isDirt == true ||
      returnWorld()->isNearBoulder(get_x(), get_y(), 3))
  {
    setAlive(false);
    set_visible(false);
  }
  
  else
  {
    moveDir(get_direction(), 1); //If the squirt moves, then reduce it's allowed travel distance
    m_distance--;
  }
  
}

void WaterSquirt::getAnnoyed(int howMuch)
{}

void WaterSquirt::setBribe()
{}


///////////////////////////////////////////////////////////////////////////
///////////////////-----------WATER POOL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

WaterPool::WaterPool(int x, int y, StudentWorld* world)
:Actor(IID_WATER_POOL, x, y, right, 1.0, 2, world)
{
  set_visible(true);
  m_ticks = max(100, 300 - 10 * returnWorld()->getCurrentLevel());
}

WaterPool::~WaterPool()
{}

void WaterPool::doSomething()
{
  m_ticks--;
  if (!isAlive())
    return;
  if (m_ticks <= 0) //If ticks runs out, remove pool from oil field
  {
    setAlive(false);
    set_visible(false);
    return;
  }
  if (returnWorld()->isNearFrackMan(this, 3)) //FrackMan picks up the pool of water
  {
    setAlive(false);
    returnWorld()->play_sound(SOUND_GOT_GOODIE);
    returnWorld()->setSquirts(5);
    returnWorld()->increase_score(100);
  }
}

void WaterPool::getAnnoyed(int howMuch)
{}

void WaterPool::setBribe()
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

void Dirt::doSomething()
{}

void Dirt::getAnnoyed(int howMuch)
{}

bool Dirt::canActorsPassThroughMe() const
{
  return false;
}

void Dirt::setBribe()
{}
