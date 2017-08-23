/*******************************************************************************
 *StudentWorld.h
 *
 * This routine is responsible for setting up the configuration of the game
 * (i.e. (de)initializing actors, updating actors)
 *
 * $LastChangedBy: jless $
 * $LastChangedDate: 2017-08-23 11:11:11 -0700 (Wed, 23 Aug 2017) $
 *
 ******************************************************************************/

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------INCLUDES--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////
///////////////////-----------STUDENTWORLD--------------///////////////////
///////////////////////////////////////////////////////////////////////////

/* Note: The StudentWorld class is derived from the GameWorld class to utilize
 *       some standard base functions created by Professor Smallberg.
 */

class StudentWorld : public GameWorld {
  public:
    // Constructor
    StudentWorld(std::string assetDir);
    // Destructor
    ~StudentWorld();
    // Derived pure virtual functions from GameWorld
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    // TODO: Add StudentWorld Functions
    bool isDirt(int x, int y);
    bool removeDirt(int x, int y, int direction);
    
  private:
    // TODO: Add StudentWorld variables
    std::vector<Actor*> m_actor;
    Dirt* m_dirt[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // STUDENTWORLD_H_






