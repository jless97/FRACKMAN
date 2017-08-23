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
//////////////////////-----------GLOBALS--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#define GRID_WIDTH 64
#define GRID_HEIGHTH 60

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
    /* Derived pure virtual functions from GameWorld (called automatically by GameWorld) */
      // Function used to initialize the oil field by populating it with the actors
      // Note: This function is called automatically by the GameWorld object
    virtual int init();
      // Function to be called each clock tick to update the actors in the oil field
      // In addition, cleans up dead actors (which is done after updating the actors)
      // Actors update at a frequency of 10-20 Hz (as specified by GameWorld)
    virtual int move();
      // Function to clear out active actors in the oil field
      // Called when the player loses a life, or completes the current level
    virtual void cleanUp();
    // TODO: Add StudentWorld Functions
    bool isDirt(int x, int y);
    bool removeDirt(int x, int y, int direction);
    
  private:
    // Vector container to hold all actors (except FrackMan and dirt)
    std::vector<Actor*> m_actor;
    // 2D Array of dirt pointers
    Dirt* m_dirt[VIEW_WIDTH][VIEW_HEIGHT];
    // FrackMan pointer
    FrackMan* m_frackman;
};

#endif // STUDENTWORLD_H_






