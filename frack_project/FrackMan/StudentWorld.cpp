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
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <cmath>
using namespace std;

///////////////////////////////////////////////////////////////////////////
///////////////////-----------STUDENTWORLD--------------///////////////////
///////////////////////////////////////////////////////////////////////////

GameWorld* createStudentWorld(string asset_dir) { return new StudentWorld(asset_dir); }

///////////////////////////////////////////////////////////////////////////
/////////////-----------CONSTRUCTOR/DESTRUCTOR--------------///////////////
///////////////////////////////////////////////////////////////////////////

StudentWorld::StudentWorld(std::string asset_dir)
:GameWorld(asset_dir), m_currentlevel(0) {}

StudentWorld::~StudentWorld() { clean_up(); }

///////////////////////////////////////////////////////////////////////////
///////////-----------GAMEWORLD: VIRTUAL FUNCTIONS--------------///////////
///////////////////////////////////////////////////////////////////////////

int StudentWorld::init() {
  // initialize member variables
  m_ticks = 0;
  m_barrels = 0;
  m_protesters = 0;
  
  // Initialize the frackman
  m_Frackman = new Frackman(30, 60, this);
  
  // Initialize the oil field
  create_dirt();
  
  // Add initial actors
  add_initial_actors();
  
  return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
  m_ticks++;
  
  // Update status text (see new function)
  update_display_text();
  
  m_Frackman->do_something();
  if (!m_Frackman->is_alive()) {
    dec_lives();
    return GWSTATUS_PLAYER_DIED;
  }
  
  // Give each Actor a chance to do something
  for (unsigned int i = 0; i < m_actor.size(); i++) {
    if (m_actor.at(i)->is_alive()) {
      m_actor.at(i)->do_something();
    }
    else {
      // update counts
      int id = m_actor.at(i)->get_id();
      if (id == IID_BARREL) {
        m_barrels--;
      }
      if (id == IID_PROTESTER || id == IID_HARD_CORE_PROTESTER) {
        m_protesters--;
      }
    }
  }
  
  bool Frackman_died = false;
  if (!m_Frackman->is_alive()) {
    Frackman_died = true;
    dec_lives();
  }
  
  // Remove newly-dead actors after each tick
  for (unsigned int i = 0; i < m_actor.size(); i++) {
    if (!m_actor.at(i)->is_alive()) {
      delete m_actor.at(i);
      vector<Actor*>::iterator it = m_actor.begin();
      
      for (unsigned int j = 0; j < i; j++) {
        it++;
      }
      
      m_actor.erase(it);
      i--;
    }
  }
  
  // Check if end of level
  if (m_barrels == 0) {
    play_sound(SOUND_FINISHED_LEVEL);
    advance_to_next_level();
    m_currentlevel++;
    return GWSTATUS_FINISHED_LEVEL;
  }
  
  if (Frackman_died) {
    return GWSTATUS_PLAYER_DIED;
  }
  
  // add protesters, waterpools, sonars
  add_more_actors();
  
  return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::clean_up() {
  delete m_Frackman;
  
  // Delete the actors in the vector container
  for (unsigned int i = 0; i < m_actor.size(); i++) {
    delete m_actor.at(i);
  }
  
  m_actor.clear();
  
  //Delete all dirt objects
  for (int i = 0; i < VIEW_WIDTH; i++) {
    for (int j = 0; j < VIEW_HEIGHT; j++) {
      if (m_dirt[i][j] != nullptr) {
        delete m_dirt[i][j];
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////
/////////////-----------STUDENTWORLD: FUNCTIONS--------------//////////////
///////////////////////////////////////////////////////////////////////////

void StudentWorld::kill_frackman() { m_Frackman->set_dead(); }

void StudentWorld::annoy_frackman() { m_Frackman->get_annoyed(2); }

void StudentWorld::annoy_protester(Actor* a, int how_much) {
  //Check to find protester, and if it is the right protester, annoy him
  for (unsigned int i = 0; i < m_actor.size(); i++) {
    int image_id = m_actor.at(i)->get_id();
    if (image_id == IID_PROTESTER || image_id == IID_HARD_CORE_PROTESTER) {
      if (radius(a->get_x(), a->get_y(), m_actor.at(i)->get_x(), m_actor.at(i)->get_y()) <= 4) {
        m_actor.at(i)->get_annoyed(how_much);
      }
    }
  }
}

void StudentWorld::set_barrel(int how_much) { m_barrels += how_much; }

void StudentWorld::set_squirts(int how_much) { m_Frackman->set_squirts(how_much); }

void StudentWorld::set_sonars(int how_much) { m_Frackman->set_sonars(how_much); }

void StudentWorld::set_gold(int how_much) { m_Frackman->set_gold(how_much); }

void StudentWorld::set_bribe(int how_much) {
  //Find the protester, and give him the gold (bribe)
  int count = 0;
  vector<Actor*>::iterator it = m_actor.begin();
  
  while (it != m_actor.end()) {
    if (count == 1) {
      break;
    }
    if ((*it)->get_id() == IID_PROTESTER || (*it)->get_id() == IID_HARD_CORE_PROTESTER) {
      (*it)->set_bribe();
      count++;
    }
    it++;
  }
}

// Add gold (bribe) to the oil field
void StudentWorld::add_bribe(Actor* a) {
  m_actor.push_back(new Bribe(a->get_x(), a->get_y(), this));
}

void StudentWorld::add_water_squirt(Actor* a, Actor::Direction direction) {
  //Add a squirt object to the oil field, depending on coordinates and direction of Frackman
  int x = a->get_x();
  int y = a->get_y();
  
  if (direction == Actor::left) {
    if (!(is_dirt(x - 3, y) && !(is_boulder(x - 3, y, Actor::left)))) {
      m_actor.push_back(new WaterSquirt(a->get_x() - 3, a->get_y(), direction, this));
    }
  }
  if (direction == Actor::right) {
    if (!(is_dirt(x + 3, y) && !(is_boulder(x + 3, y, Actor::right)))) {
      m_actor.push_back(new WaterSquirt(a->get_x() + 3, a->get_y(), direction, this));
    }
  }
  if (direction == Actor::down) {
    if (!(is_dirt(x, y - 3) && !(is_boulder(x, y - 3, Actor::up)))) {
      m_actor.push_back(new WaterSquirt(a->get_x(), a->get_y() - 3, direction, this));
    }
  }
  if (direction == Actor::up) {
    if (!(is_dirt(x, y + 3) && !(is_boulder(x, y + 3, Actor::up)))) {
      m_actor.push_back(new WaterSquirt(a->get_x(), a->get_y() + 3, direction, this));
    }
  }
}

void StudentWorld::add_initial_actors() {
  // Add a Protester on very first tick
  m_actor.push_back(new RegularProtester(60, 60, this));
  
  // add gold nuggets
  int gold = max(5 - m_currentlevel / 2, 2);
  for (int i = 0; i < gold;) {
    int x = rand_int(0, 60);
    int y = rand_int(20, 56);
    if (is_placeable(x, y)) {
      m_actor.push_back(new Gold(x, y, this));
      i++;
    }
  }
  
  // add oil barrels
  int barrels = min(2 + m_currentlevel, 20);
  for (int i = 0; i < barrels;) {
    int x = rand_int(0, 60);
    int y = rand_int(20, 56);
    if (is_placeable(x, y)) {
      m_actor.push_back(new Barrel(x, y, this));
      m_barrels++;
      i++;
    }
  }
  
  // add boulders
  int boulders = min(m_currentlevel / 2 + 2, 6);
  for (int i = 0; i < boulders;) {
    int x = rand_int(0, 60);
    int y = rand_int(20, 56);
    if (is_placeable(x, y)) {
      m_actor.push_back(new Boulder(x, y, this));
      
      // Remove dirt around boulder
      remove_dirt(x, y);
      i++;
    }
  }
}

void StudentWorld::add_more_actors() {
  // Add protesters
  if (m_ticks >= max(25, 200 - m_currentlevel)) {
    if (m_protesters < min(15, 2 + (int)(m_currentlevel * 1.5))) {
      int odds = min(90, m_currentlevel * 10 + 30);
      if (rand() % 100 < odds) {
        m_actor.push_back(new HardcoreProtester(60, 60, this));
      }
      else {
        m_actor.push_back(new RegularProtester(60, 60, this));
      }
      m_protesters++;
      m_ticks = 0;    // reset ticks (mainly used by this function)
    }
  }
  
  // Add sonar kits and water pools
  int G = (m_currentlevel * 25) + 300;
  //1 in G chance to add sonar or waterpool
  if ((rand() % G == 0)) {
    //1 in 5 chance
    if ((rand() % 5) == 0) {
      m_actor.push_back(new Sonar(0, 60, this));
    }
    else {
      int x, y;
      do {
        x = rand() % 60;
        y = rand() % 56;
      } while ((noDirt(x,y) == false) || (check_radius(x,y) == false));
      
      m_actor.push_back(new WaterPool(x, y, this));
    }
  }
}

bool StudentWorld::is_placeable(int x, int y) {
  //Used for initial placement of boulders
  vector<Actor*>::iterator it = m_actor.begin();
  
  while (it != m_actor.end()) {
    if (radius(x, y, (*it)->get_x(), (*it)->get_y()) <= 6) {
      return false;
    }
    it++;
  }
  if ((x >= 27 && x <= 33) && (y >= 4)) {
    return false;
  }
  return true;
}

bool StudentWorld::check_radius(int x, int y) {
  //Check to make sure there isn't another actor occupying a given location
  vector<Actor*>::iterator it = m_actor.begin();
  
  while (it != m_actor.end()) {
    if (radius(x, y, (*it)->get_x(), (*it)->get_y()) <= 6) {
      return false;
    }
    it++;
  }
  return true;
}

bool StudentWorld::is_near_Frackman(Actor* a, int r) const {
  if (radius(m_Frackman->get_x(), m_Frackman->get_y(), a->get_x(), a->get_y()) <= r) {
    return true;
  }
  
  return false;
}

bool StudentWorld::is_near_protester(Actor* a, int r) {
  vector<Actor*>::iterator it = m_actor.begin();
  
  while (it != m_actor.end()) {
    if ((*it)->get_id() == IID_PROTESTER || (*it)->get_id() == IID_HARD_CORE_PROTESTER) {
      if (radius(a->get_x(), a->get_y(), (*it)->get_x(), (*it)->get_y()) <= r) {
        return true;
      }
    }
    it++;
  }
  
  return false;
}

void StudentWorld::is_near_actor(Actor* a, int r) {
  //Check to see if Frackman is near a barrel or gold
  vector<Actor*>::iterator it = m_actor.begin();
  
  while (it != m_actor.end()) {
    if ((*it)->get_id() == IID_BARREL || (*it)->get_id() == IID_GOLD) {
      if (radius(a->get_x(), a->get_y(), (*it)->get_x(), (*it)->get_y()) <= r) {
        (*it)->set_visible(true);
      }
    }
    it++;
  }
}

bool StudentWorld::isNearBoulder(int x, int y, int r) {
  //Check to see if an actor is near a boulder
  vector<Actor*>::iterator it = m_actor.begin();
  
  while (it != m_actor.end()) {
    if ((*it)->get_id() == IID_BOULDER) {
      if (radius(x, y, (*it)->get_x(), (*it)->get_y()) <= r) {
        return true;
      }
    }
    it++;
  }
  
  return false;
}

bool StudentWorld::facingTowardFrackman(Actor* a) const {
  return true;  /// to be done
}

GraphObject::Direction StudentWorld::lineOfSightToFrackman(Actor* a) const {
  //If protester is in line with the Frackman, then set its direction to that given direction
  int x = a->get_x();
  int y = a->get_y();
  
  GraphObject::Direction dir;
  
  int target_x = m_Frackman->get_x();
  int target_y = m_Frackman->get_y();
  
  dir = GraphObject::up;
  for (int i = y; i < 61; i++) {
    if (i == target_y && x == target_x) {
      return dir;
    }
  }
  dir = GraphObject::down;
  for (int i = y; i > -1; i--) {
    if (i == target_y && x == target_x) {
      return dir;
    }
  }
  dir = GraphObject::left;
  for (int i = x; i > -1; i--) {
    if (i == target_x && y == target_y) {
      return dir;
    }
  }
  dir = GraphObject::right;
  for (int i = x; i < 61; i++) {
    if (i == target_x && y == target_y) {
      return dir;
    }
  }
  
  //If not in line, return direction none
  return GraphObject::none;
}

/*
 GraphObject::Direction StudentWorld::DirToFrackman(Actor* a) const
 {
	int x = a->get_x();
	int y = a->get_y();
	
	GraphObject::Direction dir;
 
	int target_x = m_Frackman->get_x();
	int target_y = m_Frackman->get_y();
 
	dir = GraphObject::up;
	for (int i = y; i < 60; i++) {
 if (i == target_y)
 return dir;
	}
	dir = GraphObject::down;
	for (int i = y; i > 0; i--) {
 if (i == target_y)
 return dir;
	}
	dir = GraphObject::left;
	for (int i = x; i > 0; i--) {
 if (i == target_x)
 return dir;
	}
	dir = GraphObject::right;
	for (int i = x; i < 60; i++) {
 if (i == target_x)
 return dir;
	}
 
	return GraphObject::none;
 }
 */

bool StudentWorld::can_actor_move_to(Actor* a, int x, int y) const {
  if (x < 0 || x > 60 || y < 0 || y > 60) {
    return false;
  }
  
  if (is_dirt(x, y)) {
    return false;
  }
  
  if (is_boulder(x,y, a->get_direction())) {
    return false;
  }
  
  return true;
}


void StudentWorld::update_display_text() {
  int score = get_score();
  if (score < 0) {
    score = 0;
  }
  string text =
		"Scr: " + std::string(6 - to_string(score).length(), '0') + to_string(score) + "  "
		+ "Lvl: " + std::string(2 - to_string(m_currentlevel).length(), ' ') + to_string(m_currentlevel) + "  "
		+ "Lives: " + to_string(get_lives()) + "  "
		+ "Hlth: " + std::string(3 - to_string(m_Frackman->get_health() * 10).length(), ' ') + to_string(m_Frackman->get_health() * 10) + "%  "
		+ "Wtr: " + std::string(2 - to_string(m_Frackman->getSquirts()).length(), ' ') + to_string(m_Frackman->getSquirts()) + "  "
		+ "Gld: " + std::string(2 - to_string(m_Frackman->get_gold()).length(), ' ') + to_string(m_Frackman->get_gold()) + "  "
		+ "Sonar: " + std::string(2 - to_string(m_Frackman->get_sonars()).length(), ' ') + to_string(m_Frackman->get_sonars()) + "  "
		+ "Oil Left: " + std::string(2 - to_string(m_barrels).length(), ' ') + to_string(m_barrels);
  
  set_game_stat_text(text);
}

//Was having a problem with the getLevel()
int StudentWorld::getCurrentLevel() const { return m_currentlevel; }

double StudentWorld::radius(int x1, int y1, int x2, int y2) const {
  int xdif = x2 - x1;
  int ydif = y2 - y1;
  int xsquared = xdif * xdif;
  int ysquared = ydif * ydif;
  double distance = sqrt(xsquared + ysquared);
  
  return distance;
}

int StudentWorld::rand_int(int min, int max) {
  //Generate a random number (taken from project 1)
  if (max < min) {
    swap(max, min);
  }
  
  static random_device rd;
  static mt19937 generator(rd());
  uniform_int_distribution<> distro(min, max);
  
  return distro(generator);
}

void StudentWorld::create_dirt() {
  //Create all the dirt objects and set their visible/invisible status
  // Initialize left-side of oil field
  for (int i = 0; i < 30; i++) {
    for (int j = 0; j < VIEW_HEIGHT - 4; j++) {
      m_dirt[i][j] = new Dirt(i, j, this);
    }
    for (int j = VIEW_HEIGHT - 4; j < VIEW_HEIGHT; j++) {
      m_dirt[i][j] = new Dirt(i, j, this);
      m_dirt[i][j]->set_visible(false);
    }
  }
  // Initialize mid-section of oil field
  for (int i = 30; i < 34; i++) {
    for (int j = 0; j < 4; j++) {
      m_dirt[i][j] = new Dirt(i, j, this);
    }
    for (int j = 4; j < VIEW_HEIGHT; j++) {
      m_dirt[i][j] = new Dirt(i, j, this);
      m_dirt[i][j]->set_visible(false);
    }
  }
  // Initialize right-side of oil field
  for (int i = 34; i < VIEW_WIDTH; i++) {
    for (int j = 0; j < VIEW_HEIGHT - 4; j++) {
      m_dirt[i][j] = new Dirt(i, j, this);
    }
    for (int j = VIEW_HEIGHT - 4; j < VIEW_HEIGHT; j++) {
      m_dirt[i][j] = new Dirt(i, j, this);
      m_dirt[i][j]->set_visible(false);
    }
  }
}

bool StudentWorld::is_dirt(int x, int y) const {
  if (x < 0 || x > 60 || y < 0 || y > 60) {
    return false;
  }
  if (m_dirt[x][y] != nullptr && m_dirt[x][y]->is_visible()) {
    return true;
  }
  
  return false;
}

bool StudentWorld::is_dirt(int x, int y, int direction) const {
  if (x < 0 || x > 60 || y < 0 || y > 60) {
    return false;
  }
  
  //Based off of direction and coordinates, check fours squares in that direction for dirt
  bool dirt_present = false;
  switch (direction) {
    case GraphObject::left:
      if ((m_dirt[x - 1][y] != nullptr && m_dirt[x - 1][y]->is_visible()) ||
          (m_dirt[x - 1][y + 1] != nullptr && m_dirt[x - 1][y + 1]->is_visible()) ||
          (m_dirt[x - 1][y + 2] != nullptr && m_dirt[x - 1][y + 2]->is_visible()) ||
          (m_dirt[x - 1][y + 3] != nullptr && m_dirt[x - 1][y + 3]->is_visible())) {
        dirt_present = true;
      }
      break;
    case GraphObject::right:
      if ((m_dirt[x + 4][y] != nullptr && m_dirt[x + 4][y]->is_visible()) ||
          (m_dirt[x + 4][y + 1] != nullptr && m_dirt[x + 4][y + 1]->is_visible()) ||
          (m_dirt[x + 4][y + 2] != nullptr && m_dirt[x + 4][y + 2]->is_visible()) ||
          (m_dirt[x + 4][y + 3] != nullptr && m_dirt[x + 4][y + 3]->is_visible())) {
        dirt_present = true;
      }
      break;
    case GraphObject::down:
      if ((m_dirt[x][y - 1] != nullptr && m_dirt[x][y - 1]->is_visible()) ||
          (m_dirt[x + 1][y - 1] != nullptr && m_dirt[x + 1][y - 1]->is_visible()) ||
          (m_dirt[x + 2][y - 1] != nullptr && m_dirt[x + 2][y - 1]->is_visible()) ||
          (m_dirt[x + 3][y - 1] != nullptr && m_dirt[x + 3][y - 1]->is_visible())) {
        dirt_present = true;
      }
      break;
    case GraphObject::up:
      if ((m_dirt[x][y + 4] != nullptr && m_dirt[x][y + 4]->is_visible()) ||
          (m_dirt[x + 1][y + 4] != nullptr && m_dirt[x + 1][y + 4]->is_visible()) ||
          (m_dirt[x + 2][y + 4] != nullptr && m_dirt[x + 2][y + 4]->is_visible()) ||
          (m_dirt[x + 3][y + 4] != nullptr && m_dirt[x + 3][y + 4]->is_visible())) {
        dirt_present = true;
      }
      break;
  }
  
  if (dirt_present) {
    return true;
  }
  
  return false;
}


bool StudentWorld::noDirt(int start_x, int start_y) const {
  if (start_x> VIEW_WIDTH - 4 || start_y> VIEW_HEIGHT - 4) {
    return false;
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (is_dirt(start_x + i, start_y + j)) {
        return false;
      }
    }
  }
  
  return true;
}

bool StudentWorld::is_boulder(int x, int y, int direction) const {
  if (x < 0 || x > 60 || y < 0 || y > 60) {
    return false;
  }
  
  //Similar to 3 parameter is_dirt, check a 4x4 region for a boulder
  bool boulder_present = false;
  for (unsigned int i = 0; i < m_actor.size(); i++) {
    int image_id = m_actor.at(i)->get_id();
    if (image_id == IID_BOULDER) {
      int a = m_actor.at(i)->get_x();
      int b = m_actor.at(i)->get_y();
      switch (direction) {
        case KEY_PRESS_LEFT:
          for (int i = 0; i < 4; i++) {
            if (((x - 3) == a && (y + i) == b) ||
                ((x - 3) == a && (y + i) == b + 1) ||
                ((x - 3) == a && (y + i) == b + 1) ||
                ((x - 3) == a && (y + i) == b + 3)) {
              boulder_present = true;
            }
          }
          break;
        case KEY_PRESS_RIGHT:
          for (int i = 0; i < 4; i++) {
            if (((x + 3) == a && (y + i) == b) ||
                ((x + 3) == a && (y + i) == b + 1) ||
                ((x + 3) == a && (y + i) == b + 1) ||
                ((x + 3) == a && (y + i) == b + 3)) {
              boulder_present = true;
            }
          }
          break;
        case KEY_PRESS_UP:
          for (int i = 0; i < 4; i++) {
            if (((x + i) == a && (y + 3) == b) ||
                ((x + i) == a + 1 && (y + 3) == b) ||
                ((x + i) == a + 2 && (y + 3) == b) ||
                ((x + i) == a + 3 && (y + 3) == b)) {
              boulder_present = true;
            }
          }
          break;
        case KEY_PRESS_DOWN:
          for (int i = 0; i < 4; i++) {
            if (((x + i) == a && (y - 3) == b) ||
                ((x + i) == a + 1 && (y - 3) == b) ||
                ((x + i) == a + 2 && (y - 3) == b) ||
                ((x + i) == a + 3 && (y - 3) == b)) {
              boulder_present = true;
            }
          }
          break;
      }
    }
  }
  
  if (boulder_present == true) {
    return true;
  }
  
  return false;
}

bool StudentWorld::remove_dirt(int x, int y, int direction) {
  //To keep track of when dirt is being mined (set to invisible)
  bool play_sound = false;
  
  //Upon moving either left,right,up, or down, check to see if there is dirt there
  switch (direction) {
    case KEY_PRESS_LEFT:
      for (int i = 0; i < 4; i++) {
        if (is_dirt(x, y + i)) {
          play_sound = true;
          m_dirt[x][y + i]->set_visible(false);
        }
      }
      break;
    case KEY_PRESS_RIGHT:
      for (int i = 0; i < 4; i++) {
        if (is_dirt(x + 3, y + i)) {
          m_dirt[x + 3][y + i]->set_visible(false);
          play_sound = true;
        }
      }
      break;
    case KEY_PRESS_UP:
      for (int i = 0; i < 4; i++) {
        if (is_dirt(x + i, y + 3)) {
          m_dirt[x + i][y + 3]->set_visible(false);
          play_sound = true;
        }
      }
      break;
    case KEY_PRESS_DOWN:
      for (int i = 0; i < 4; i++) {
        if (is_dirt(x + i, y)) {
          m_dirt[x + i][y]->set_visible(false);
          play_sound = true;
        }
      }
      break;
  }
  
  //If dirt was ever mined by Frackman (set to invisible), return true to play sound
  if (play_sound) {
    return true;
  }
  
  return false;
}

void StudentWorld::remove_dirt(int x, int y) {
  for (int i = x; i < x + 4; i++) {
    for (int j = y; j < y + 4; j++) {
      m_dirt[i][j]->set_visible(false);
    }
  }
}
