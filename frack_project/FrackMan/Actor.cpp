#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////////
//  Actor implementation
///////////////////////////////////////////////////////////////////////////

Actor::Actor(int imageID, int startX, int startY, Direction startDirection,
             float size, unsigned int depth,StudentWorld* world)
:GraphObject(imageID, startX, startY, startDirection, size, depth), m_world(world)
{
    setVisible(false);
}

Actor::~Actor()
{
    setVisible(false);
}

StudentWorld* Actor::returnWorld() const
{
    return m_world;
}

///////////////////////////////////////////////////////////////////////////
//  FrackMan implementation
///////////////////////////////////////////////////////////////////////////

FrackMan::FrackMan(StudentWorld* world)
  :Actor(IID_PLAYER, 30, 60, right, 1.0, 0, world), m_health(10), m_squirts(5), m_sonars(1), m_gold(0)
{
   setVisible(true);
}

FrackMan::~FrackMan()
{}

void FrackMan::doSomething()
{
    if (!isAlive()) //Check to see if FrackMan is alive
        return;
    int x = getX();
    int y = getY();
    returnWorld()->removeDirt(x,y,none); //If FrackMan spawns in a dirt location, remove the dirt
    int ch;
    if (returnWorld()->getKey(ch) == true) //Check to see if a valid key was input by the user
    {
        switch (ch) //To see what FrackMan is going to do in the given turn (tick)
        {
            case KEY_PRESS_LEFT:
                if (getDirection() == left)
                    moveFrackMan(ch);
                else
                    setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() == right)
                    moveFrackMan(ch);
                else
                    setDirection(right);
                break;
            case KEY_PRESS_UP:
                if (getDirection() == up)
                    moveFrackMan(ch);
                else
                    setDirection(up);
                break;
            case KEY_PRESS_DOWN:
                if (getDirection() == down)
                    moveFrackMan(ch);
                else
                    setDirection(down);
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

bool FrackMan::isAlive() const
{
    return m_health > 0;
}

bool FrackMan::validPosition(int x, int y) const
{
    if (x < 0 || x > 60 || y < 0 || y > 60)
        return false;
    return true;
}

void FrackMan::moveFrackMan(int dir)
{
    int x = getX();
    int y = getY();
    if (validPosition(x, y) == false)
        return;
    switch (dir) //To see what direction to move FrackMan, and if the SOUND_DIG should play
    {
        case KEY_PRESS_LEFT:
            if (x != 0)
            {
                if (returnWorld()->removeDirt(x - 1, y, KEY_PRESS_LEFT)) //If dirt was removed, play sound
                    returnWorld()->playSound(SOUND_DIG);
                moveTo(x - 1, y);
            }
            if (x == 0)
                moveTo(x,y);
            break;
        case KEY_PRESS_RIGHT:
            if (x != 60)
            {
                if (returnWorld()->removeDirt(x + 1, y, KEY_PRESS_RIGHT))
                    returnWorld()->playSound(SOUND_DIG);
                moveTo(x + 1, y);
            }
            if (x == 60)
                moveTo(x,y);
            break;
        case KEY_PRESS_DOWN:
            if (y != 0)
            {
                if (returnWorld()->removeDirt(x, y - 1, KEY_PRESS_DOWN))
                    returnWorld()->playSound(SOUND_DIG);
                moveTo(x, y - 1);
            }
            if (y == 0)
                moveTo(x,y);
            break;
        case KEY_PRESS_UP:
            if (y != 60)
            {
                if (returnWorld()->removeDirt(x, y + 1, KEY_PRESS_UP))
                    returnWorld()->playSound(SOUND_DIG);
                moveTo(x, y + 1);
            }
            if (y == 60)
                moveTo(x,y);
            break;
    }
    
}

///////////////////////////////////////////////////////////////////////////
//  Dirt implementation
///////////////////////////////////////////////////////////////////////////

Dirt::Dirt(StudentWorld* world, int x, int y)
:Actor(IID_DIRT, x, y, right, 0.25, 3, world)
{
    setVisible(true);
}

Dirt::~Dirt()
{}

void Dirt::doSomething()
{}
