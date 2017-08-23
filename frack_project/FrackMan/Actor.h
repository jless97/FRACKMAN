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
//////////////////////-----------GLOBALS--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#define EDGE_LOWER 0
#define EDGE_UPPER 60

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------ACTOR--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

class StudentWorld;

/* Derived from GraphObject:
 int        m_image_id;          // Supply image ID
 bool       m_visible;          // Objects start out invisible
 double     m_x;                // Supply initial x-coord
 double     m_y;                // Supply initial y-coord
 double     m_destX;            // Same as m_x
 double     m_destY;            // Same as m_y
 double     m_brightness;       // Objects have brightness of 1
 int		m_animationNumber;      // Objects have animation of 0
 Direction	m_direction;        // Supply initial direction
 double     m_size;             // Supply object's size
 int		m_depth;                // Supply the depth of the object
 Additional Actor objects:
 StudentWorld* world;           // Each actor needs to point to the oil field object
*/
class Actor : public GraphObject {
  public:
    // Constructor
    Actor(int image_id, int start_x, int start_y, Direction start_direction,
          float size, UINT depth, StudentWorld* world);
    // Destructor
    virtual ~Actor();
    // Pure virtual (as each derived actor object does a different doSomething function)
    virtual void do_something() = 0;
    StudentWorld* return_world() const;
    
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
//set_visible(true)
class FrackMan : public Actor {
  public:
    // Constructor
    FrackMan(StudentWorld* world);
    // Destructor
    virtual ~FrackMan();
    // Derived virtual functions
    virtual void do_something();
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
//set_visible(true)
class Dirt : public Actor {
  public:
    // Constructor
    Dirt(StudentWorld* world, int x, int y);
    // Destructor
    virtual ~Dirt();
    // Derived virtual functions
    virtual void do_something();
    // TODO: Add Dirt functions
    
  private:
    // TODO: Add Dirt variables
};

#endif // ACTOR_H_













