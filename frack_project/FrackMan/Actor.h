//Actor.h

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
  public:
    Actor(int imageID, int startX, int startY, Direction startDirection,
          float size, unsigned int depth, StudentWorld* world);
    virtual ~Actor();
    virtual void doSomething() = 0;
    StudentWorld* returnWorld() const;
    
  private:
    StudentWorld* m_world;
};

//Image ID : IID_PLAYER
//Start Location : x = 30, y = 60
//State: 10 hit points, 5 water units, 1 sonar charge, 0 gold nuggets, Direction: right
//Depth = 0 (Foreground), size = 1.0;
//setVisible(true)
class FrackMan : public Actor
{
  public:
    FrackMan(StudentWorld* world);
    virtual ~FrackMan();
    virtual void doSomething();
    bool isAlive() const;
    bool validPosition(int x, int y) const;
    void moveFrackMan(int dir);

  private:
    int m_health;
    int m_squirts;
    int m_sonars;
    int m_gold;
};

//Image ID : IID_DIRT
//Direction : RIGHT
//depth = 3; (background)
//size = .25 (1x1 square in the oil field)
//setVisible(true)
class Dirt : public Actor
{
  public:
    Dirt(StudentWorld* world, int x, int y);
    virtual ~Dirt();
    virtual void doSomething();
    
  private:
};

#endif // ACTOR_H_
