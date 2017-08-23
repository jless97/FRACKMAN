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

// Creates a game world object pointer, which points to this student world object (used in main.c)
GameWorld* createStudentWorld(string assetDir) {
	return new StudentWorld(assetDir);
}

///////////////////////////////////////////////////////////////////////////
/////////////-----------CONSTRUCTOR/DESTRUCTOR--------------///////////////
///////////////////////////////////////////////////////////////////////////

/* StudentWorld default constructor that needs a parameter (i.e. the assets directory path)
 * to initialize the GameWorld object
 */
StudentWorld::StudentWorld(std::string assetDir)
  :GameWorld(assetDir) {
}

// StudentWorld destructor to free all memory consumed by the actors of the game
StudentWorld::~StudentWorld() {
  //Delete all actors
  for (int i = 0; i < m_actor.size(); i++) {
    delete m_actor.at(i);
  }
  
  m_actor.clear();

  //Delete all dirt objects
  for (int i = 0; i < 30; i++) {
    for (int j = 0; j < GRID_HEIGHTH; j++) {
      delete m_dirt[i][j];
    }
  }
  
  for (int i = 30; i < 34; i++) {
    for (int j = 0; j < 4; j++) {
      delete m_dirt[i][j];
    }
  }
  for (int i = 34; i < VIEW_WIDTH; i++) {
    for (int j = 0; j < GRID_HEIGHTH; j++) {
      delete m_dirt[i][j];
    }
  }
}

///////////////////////////////////////////////////////////////////////////
///////////-----------GAMEWORLD: VIRTUAL FUNCTIONS--------------///////////
///////////////////////////////////////////////////////////////////////////

/* DESCRIPTION:
 A. Initialize the data structures used to keep track of your game’s virtual world
 B. Construct a new oil field that meets the requirements stated in the section below
 (filled with Dirt, Barrels of oil, Boulders, Gold Nuggets, etc.)
 C. Allocate and insert a valid FrackMan object into the game world at the proper
 location
 */
int StudentWorld::init() {
  //Create the FrackMan player, which points to the StudentWorld object
  m_frackman = new FrackMan(this);
  
  // Push the FrackMan object into the actor vector
  m_actor.push_back(m_frackman);
  
  /* Create all the dirt objects and set their visible/invisible status
   * Note: Oil Field grows left to right, and bottom to top (i.e. x = 0, y = 0 is bottom-left of grid)
  */
  // Initialize left-side of oil field
  for (int i = 0; i < 30; i++) {
    for (int j = 0; j < GRID_HEIGHTH; j++) {
      m_dirt[i][j] = new Dirt(this, i, j);
      m_dirt[i][j]->set_visible(true);
    }
  }
  // Initialize empty area in the middle of the oil field
  for (int i = 30; i < 34; i++) {
    for (int j = 0; j < 4; j++){
      m_dirt[i][j] = new Dirt(this, i, j);
      m_dirt[i][j]->set_visible(true);
    }
  }
  // Initialize right-side of the oil field
  for (int i = 34; i < VIEW_WIDTH; i++) {
    for (int j = 0; j < GRID_HEIGHTH; j++) {
      m_dirt[i][j] = new Dirt(this, i, j);
      m_dirt[i][j]->set_visible(true);
    }
  }

  return 0;
}

int StudentWorld::move() {
  //For each actor, have them call the doSomething function
  for (int i = 0; i < m_actor.size(); i++) {
    m_actor.at(i)->do_something();
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
    for (int j = 0; j < GRID_HEIGHTH; j++) {
      delete m_dirt[i][j];
    }
  }
  for (int i = 30; i < 34; i++) {
    for (int j = 0; j < 4; j++) {
      delete m_dirt[i][j];
    }
  }
  for (int i = 34; i < VIEW_WIDTH; i++) {
    for (int j = 0; j < GRID_HEIGHTH; j++) {
      delete m_dirt[i][j];
    }
  }
}

///////////////////////////////////////////////////////////////////////////
//////////////-----------STUDENTWORLD FUNCTIONS--------------//////////////
///////////////////////////////////////////////////////////////////////////

//Check to see if there is dirt in a given location
bool StudentWorld::isDirt(int x, int y) {
  if (m_dirt[x][y]->is_visible()) {
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
            m_dirt[x][y + i]->set_visible(false);
            playSound = true;
        }
        if (isDirt(x + 1, y + i))
        {
            m_dirt[x + 1][y + i]->set_visible(false);
            playSound = true;
        }
        if (isDirt(x + 2, y + i))
        {
            m_dirt[x + 2][y + i]->set_visible(false);
            playSound = true;
        }
        if (isDirt(x + 3, y + i))
        {
            m_dirt[x + 3][y + i]->set_visible(false);
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
                    m_dirt[x][y + i]->set_visible(false);
                }
            break;
        case KEY_PRESS_RIGHT:
            for (int i = 0; i < 4; i++)
                if (isDirt(x + 3, y + i))
                {
                    m_dirt[x + 3][y + i]->set_visible(false);
                    playSound = true;
                }
            break;
        case KEY_PRESS_UP:
            for (int i = 0; i < 4; i++)
                if (isDirt(x + i, y + 3))
                {
                    m_dirt[x + i][y + 3]->set_visible(false);
                    playSound = true;
                }
            break;
        case KEY_PRESS_DOWN:
            for (int i = 0; i < 4; i++)
                if (isDirt(x + i, y))
                {
                    m_dirt[x + i][y]->set_visible(false);
                    playSound = true;
                }
            break;
    }
    if (playSound == true) //If dirt was ever mined by FrackMan (set to invisible), return true to play sound
        return true;
    return false;
}
