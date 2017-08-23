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

#include "globals.h"
#include "GraphObject.h"

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------ACTOR--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

class StudentWorld;

class Actor : public GraphObject {
  public:
    // Constructor
    Actor(int imageID, int startX, int startY, Direction startDirection,
          float size, UINT depth, StudentWorld* world);
    // Destructor
    virtual ~Actor();
    // TODO: Add (pure) virtual functions
    virtual void doSomething() = 0;
    StudentWorld* returnWorld() const;
    
  private:
    // TODO: Add Actor variables
    StudentWorld* m_world;
};

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------FRACKMAN--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

//Image ID : IID_PLAYER
//Start Location : x = 30, y = 60
//State: 10 hit points, 5 water units, 1 sonar charge, 0 gold nuggets, Direction: right
//Depth = 0 (Foreground), size = 1.0;
//setVisible(true)
class FrackMan : public Actor {
  public:
    // Constructor
    FrackMan(StudentWorld* world);
    // Destructor
    virtual ~FrackMan();
    // Derived virtual functions
    virtual void doSomething();
    // TODO: Add FrackMan functions
    bool isAlive() const;
    bool validPosition(int x, int y) const;
    void moveFrackMan(int dir);

  private:
    int m_health;
    int m_squirts;
    int m_sonars;
    int m_gold;
};

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------DIRT--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

//Image ID : IID_DIRT
//Direction : RIGHT
//depth = 3; (background)
//size = .25 (1x1 square in the oil field)
//setVisible(true)
class Dirt : public Actor {
  public:
    // Constructor
    Dirt(StudentWorld* world, int x, int y);
    // Destructor
    virtual ~Dirt();
    // Derived virtual functions
    virtual void doSomething();
    // TODO: Add Dirt functions
    
  private:
    // TODO: Add Dirt variables
};

#endif // ACTOR_H_













