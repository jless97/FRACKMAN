//StudentWorld.h

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>

class StudentWorld : public GameWorld
{
  public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool isDirt(int x, int y);
    bool removeDirt(int x, int y, int direction);

  private:
    std::vector<Actor*> m_actor;
    Dirt* m_dirt[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // STUDENTWORLD_H_
