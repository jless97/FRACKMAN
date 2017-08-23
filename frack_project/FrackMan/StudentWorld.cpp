/*******************************************************************************
 *StudentWorld.cpp
 *
 * This routine is responsible for setting up the configuration of the game
 * (i.e. (de)initializing actors, updating actors)
 *
 * $LastChangedBy: jless $
 * $LastChangedDate: 2017-08-23 11:11:11 -0700 (Wed, 23 Aug 2017) $
 *
 ******************************************************************************/

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------INCLUDES--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#include "StudentWorld.h"
#include <string>
using namespace std;

///////////////////////////////////////////////////////////////////////////
///////////////////-----------STUDENTWORLD--------------///////////////////
///////////////////////////////////////////////////////////////////////////

GameWorld* createStudentWorld(string assetDir) {
	return new StudentWorld(assetDir);
}

///////////////////////////////////////////////////////////////////////////
/////////////-----------CONSTRUCTOR/DESTRUCTOR--------------///////////////
///////////////////////////////////////////////////////////////////////////

StudentWorld::StudentWorld(std::string assetDir)
  :GameWorld(assetDir) {
}

StudentWorld::~StudentWorld() {
  //Delete all actors
  for (int i = 0; i < m_actor.size(); i++) {
    delete m_actor.at(i);
  }
  m_actor.clear();

  //Delete all dirt objects
  for (int i = 0; i < 30; i++) {
    for (int j = 0; j < VIEW_HEIGHT - 4; j++) {
      delete m_dirt[i][j];
    }
    for (int j = VIEW_HEIGHT - 4; j < VIEW_HEIGHT; j++) {
      delete m_dirt[i][j];
    }
  }
  
  for (int i = 30; i < 34; i++) {
    for (int j = 0; j < 4; j++) {
      delete m_dirt[i][j];
    }
    for (int j = 4; j < VIEW_HEIGHT; j++) {
      delete m_dirt[i][j];
    }
  }
  for (int i = 34; i < VIEW_WIDTH; i++) {
    for (int j = 0; j < VIEW_HEIGHT - 4; j++) {
      delete m_dirt[i][j];
    }
    for (int j = VIEW_HEIGHT - 4; j < VIEW_HEIGHT; j++) {
      delete m_dirt[i][j];
    }
  }
}

///////////////////////////////////////////////////////////////////////////
///////////-----------GAMEWORLD: VIRTUAL FUNCTIONS--------------///////////
///////////////////////////////////////////////////////////////////////////

int StudentWorld::init() {
  //Create the FrackMan object and push onto the vector container
  FrackMan* frackMan = new FrackMan(this);
  m_actor.push_back(frackMan);
  
  for (int i = 0; i < 30; i++) //Create all the dirt objects and set their visible/invisible status
  {
    for (int j = 0; j < VIEW_HEIGHT - 4; j++)
    {
        m_dirt[i][j] = new Dirt(this, i, j);
    }
    for (int j = VIEW_HEIGHT - 4; j < VIEW_HEIGHT; j++)
    {
        m_dirt[i][j] = new Dirt(this, i, j);
        m_dirt[i][j]->setVisible(false);
    }
  }
  for (int i = 30; i < 34; i++)
  {
    for (int j = 0; j < 4; j++)
    {
        m_dirt[i][j] = new Dirt(this, i, j);
    }
    for (int j = 4; j < VIEW_HEIGHT; j++)
    {
        m_dirt[i][j] = new Dirt(this, i , j);
        m_dirt[i][j]->setVisible(false);
    }
  }
  for (int i = 34; i < VIEW_WIDTH; i++)
  {
    for (int j = 0; j < VIEW_HEIGHT - 4; j++)
    {
      m_dirt[i][j] = new Dirt(this, i, j);
    }
    for (int j = VIEW_HEIGHT - 4; j < VIEW_HEIGHT; j++)
    {
      m_dirt[i][j] = new Dirt(this, i, j);
      m_dirt[i][j]->setVisible(false);
    }
  }

  return 0;
}

int StudentWorld::move() {
  //For each actor, have them call the doSomething function
  for (int i = 0; i < m_actor.size(); i++) {
    m_actor.at(i)->doSomething();
  }
  
  return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
  //Delete the actors in the vector container
  for (int i = 0; i < m_actor.size(); i++) {
    delete m_actor.at(i);
  }
  
  m_actor.clear();
  
  //Delete all the dirt objects
  for (int i = 0; i < 30; i++) {
    for (int j = 0; j < VIEW_HEIGHT - 4; j++) {
      delete m_dirt[i][j];
    }
    for (int j = VIEW_HEIGHT - 4; j < VIEW_HEIGHT; j++) {
      delete m_dirt[i][j];
    }
  }
  for (int i = 30; i < 34; i++) {
    for (int j = 0; j < 4; j++) {
      delete m_dirt[i][j];
    }
    for (int j = 4; j < VIEW_HEIGHT; j++) {
      delete m_dirt[i][j];
    }
  }
  for (int i = 34; i < VIEW_WIDTH; i++) {
    for (int j = 0; j < VIEW_HEIGHT - 4; j++) {
      delete m_dirt[i][j];
    }
    for (int j = VIEW_HEIGHT - 4; j < VIEW_HEIGHT; j++) {
      delete m_dirt[i][j];
    }
  }
}

///////////////////////////////////////////////////////////////////////////
//////////////-----------STUDENTWORLD FUNCTIONS--------------//////////////
///////////////////////////////////////////////////////////////////////////

//Check to see if there is dirt in a given location
bool StudentWorld::isDirt(int x, int y) {
  if (m_dirt[x][y]->isVisible()) {
    return true;
  }
  
  return false;
}

bool StudentWorld::removeDirt(int x, int y, int direction)
{
    bool playSound = false; //To keep track of when dirt is being mined (set to invisible)
    
    for (int i = 0; i < 4; i++) //Check to see if FrackMan spawns in dirt location
    {
        if (isDirt(x, y + i)) //If FrackMan were to be placed in dirt to begin, set all dirt to invisible
        {
            m_dirt[x][y + i]->setVisible(false);
            playSound = true;
        }
        if (isDirt(x + 1, y + i))
        {
            m_dirt[x + 1][y + i]->setVisible(false);
            playSound = true;
        }
        if (isDirt(x + 2, y + i))
        {
            m_dirt[x + 2][y + i]->setVisible(false);
            playSound = true;
        }
        if (isDirt(x + 3, y + i))
        {
            m_dirt[x + 3][y + i]->setVisible(false);
            playSound = true;
        }
    }
    
    switch (direction) //Upon moving either left,right,up, or down, check to see if there is dirt there
    {
        case KEY_PRESS_LEFT:
            for (int i = 0; i < 4; i++)
                if (isDirt(x, y + i))
                {
                    playSound = true;
                    m_dirt[x][y + i]->setVisible(false);
                }
            break;
        case KEY_PRESS_RIGHT:
            for (int i = 0; i < 4; i++)
                if (isDirt(x + 3, y + i))
                {
                    m_dirt[x + 3][y + i]->setVisible(false);
                    playSound = true;
                }
            break;
        case KEY_PRESS_UP:
            for (int i = 0; i < 4; i++)
                if (isDirt(x + i, y + 3))
                {
                    m_dirt[x + i][y + 3]->setVisible(false);
                    playSound = true;
                }
            break;
        case KEY_PRESS_DOWN:
            for (int i = 0; i < 4; i++)
                if (isDirt(x + i, y))
                {
                    m_dirt[x + i][y]->setVisible(false);
                    playSound = true;
                }
            break;
    }
    if (playSound == true) //If dirt was ever mined by FrackMan (set to invisible), return true to play sound
        return true;
    return false;
}
