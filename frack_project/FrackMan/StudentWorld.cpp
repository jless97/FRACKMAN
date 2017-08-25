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
#include "math.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////
///////////////////-----------STUDENTWORLD--------------///////////////////
///////////////////////////////////////////////////////////////////////////

GameWorld* createStudentWorld(string asset_dir) { return new StudentWorld(asset_dir); }

///////////////////////////////////////////////////////////////////////////
/////////////-----------CONSTRUCTOR/DESTRUCTOR--------------///////////////
///////////////////////////////////////////////////////////////////////////

StudentWorld::StudentWorld(std::string asset_dir)
:GameWorld(asset_dir) {}

StudentWorld::~StudentWorld() { clean_up(); }

///////////////////////////////////////////////////////////////////////////
//////////-----------GAMEWORLD VIRTUAL FUNCTIONS--------------/////////////
///////////////////////////////////////////////////////////////////////////

int StudentWorld::init() {
  // Initialize dirt in the oil field
  init_dirt();
  
  // Initialize frackman player
  m_frackman = new Frackman(this);

  // Add initial actors to the current level (i.e. boulders, oil barrels, and gold nuggets)
  add_initial_actors();
  
  // Initialize protester variables
  m_nprotesters = 0;
  m_nticks_since_added_protester = MAX(25, (200 - get_level()));

  return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
  // Update scoreboard
  update_scoreboard();
  
  // Simple new feature (if player score is a multiple of 50,000, add an extra live)
  static unsigned int multiple = 1;
  if (get_score() >= (EXTRA_LIVE * multiple)) { inc_lives(); multiple++; }
  
  // Give frackman a chance to do something
  m_frackman->do_something();
  
  // Give all other actors a chance to do something
  for (int i = 0; i < m_actors.size(); i++) { m_actors[i]->do_something(); }
  
  // Remove newly-dead actors after each tick
  for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ) {
    if (!(*it)->is_alive()) { delete *it; it = m_actors.erase(it); }
    else { it++; }
  }
  
  // If frackman died during this tick, decrement lives, play sound effect, and if out of lives (GameWorld goes to game over screen)
  if (!m_frackman->is_alive()) {
    play_sound(SOUND_PLAYER_GIVE_UP);
    dec_lives();
    return GWSTATUS_PLAYER_DIED;
  }
  
  // Add additional actors to the current oil field (i.e. sonar kits, water pools, and protesters)
  add_additional_actors();
  
  // Update ticks before adding another protester
  m_nticks_since_added_protester++;
  
  // If the player collected all of the barrels, advance to the next level (and play level completion sound effect)
  if (m_nbarrels == 0) {
    play_sound(SOUND_FINISHED_LEVEL);
    return GWSTATUS_FINISHED_LEVEL;
  }
  
  // Continue with the level until one of the above conditions happens
  return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::clean_up() {
  // Remove dirt
  deinit_dirt();
  
  // Remove actors
  for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ) {
    delete *it;
    it = m_actors.erase(it);
  }
  
  // Remove frackman
  delete m_frackman;
  
  return;
}

///////////////////////////////////////////////////////////////////////////
//////////////-----------ADDING ACTORS FUNCTIONS-------------//////////////
///////////////////////////////////////////////////////////////////////////

void StudentWorld::add_actor(Actor* actor) { m_actors.push_back(actor); }

void StudentWorld::add_initial_actors(void) {
  // Initialize boulders, gold nuggets, and oil barrels
  int B = MIN(get_level() / 2 + 2, 6);
  int G = MAX(5 - get_level() / 2, 2);
  int L = MIN(2 + get_level(), 20);
  
  // Place boulders
  for (int i = 0; i < B; i++) {
    int x, y;
    generate_coordinates(0, 60, 20, 56, 20, &x, &y);
    // Make sure that there are no actors within a given radius of one another
    if (!radius_from_actor(x, y, 6.00)) {
      // Add new boulder to the oil field
      Boulder* new_boulder = new Boulder(x, y, this);
      // Remove dirt surrounding new boulder
      remove_dirt(new_boulder);
    }
  }
  
  // Place oil barrels
  for (int i = 0; i < L; i++) {
    int x, y;
    bool regenerate = false;
    // Generate oil barrel coordinates
    do {
      generate_coordinates(0, 60, 0, 56, 4, &x, &y);
      // Make sure that there are no actors within a given radius of one another
      if (!radius_from_actor(x, y, 6.00)) {
        // Add new oil barrel to the oil field
        new Barrel(x, y, this);
        regenerate = false;
      }
      else { regenerate = true; }
    } while (regenerate);
  }
  
  // Place gold nuggets
  for (int i = 0; i < G; i++) {
    int x, y;
    // Generate gold nugget coordinates
    generate_coordinates(0, 60, 0, 56, 4, &x, &y);
    // Make sure that there are no actors within a given radius of one another
    if (!radius_from_actor(x, y, 6.00)) {
      // Add new gold nugget to the oil field
      new Gold(x, y, this, 0, false);
    }
  }
  
  // Set the number of oil barrels in the current level
  m_nbarrels = L;
  
  return;
}

void StudentWorld::add_additional_actors(void) {
  /* Add sonar kits and water pools */
  // There is a 1 in G chance that a new sonar kit or water pool will be added to the oil field
  int G = get_level() * 25 + 300;
  if (rand_int(1, G) == 1) {
    // Sonar kit: 1/5 chance, Water pool: 4/5 chance
    if (rand_int(1, 5) == 1) { new Sonar(this); }
    else {
      int x, y;
      generate_coordinates(0, 60, 0, 56, 0, &x, &y, true);
      new WaterPool(x, y, this);
    }
  }
  
  /* Add protesters */
  int T = MAX(25, (200 - get_level()));
  int P = MIN(15, 2 + get_level() * 1.5);
  int probability_of_hardcore = MIN(90, get_level() * 10 + 30);
  if ((m_nprotesters < P) && (m_nticks_since_added_protester >= T)) {
    // Reset protester timer
    m_nticks_since_added_protester = -1;
    // Update protester counter
    m_nprotesters++;
    // Add new protester at x = 60, y = 60
    if (rand_int(1, probability_of_hardcore) == 1) { new HardcoreProtester(this); }
    else { new Protester(this); }
  }
  
  return;
}

///////////////////////////////////////////////////////////////////////////
////////////-----------SCOREBOARD/GOODIES FUNCTIONS-------------///////////
///////////////////////////////////////////////////////////////////////////
void StudentWorld::update_scoreboard() {
  // Update scoreboard parameters
  int score = get_score();
  int level = get_level();
  int lives = get_lives();
  int health = m_frackman->get_health();
  if (health < 0) { health = 0; }
  int squirts = m_frackman->get_squirts();
  int gold = m_frackman->get_gold();
  int sonars = m_frackman->get_sonars();
  int barrels_left = m_nbarrels;
  // Convert parameters to strings
  string score_text = "Scr: " + std::string(6 - std::to_string(score).size(), '0') + std::to_string(score) + " ";
  string level_text = "Lvl: " + std::string(2 - std::to_string(level).size(), ' ') + std::to_string(level) + " ";
  string lives_text = "Lives: " + std::to_string(lives) + " ";
  string health_text = "Hlth: " + std::string(3 - std::to_string(health * 10).size(), ' ') + std::to_string(health * 10) + "%" + " ";
  string squirts_text = "Wtr: " + std::string(2 - std::to_string(squirts).size(), ' ') + std::to_string(squirts) + " ";
  string gold_text = "Gld: " + std::string(2 - std::to_string(gold).size(), ' ') + std::to_string(gold) + " ";
  string sonars_text = "Sonar: " + std::to_string(sonars) + " ";
  string barrels_text = "Oil Left: " + std::to_string(barrels_left);
  string text = score_text + level_text + lives_text + health_text + squirts_text + gold_text + sonars_text + barrels_text;
  // Display updated scoreboard
  set_game_stat_text(text);
  return;
}

void StudentWorld::dec_barrels(void) { m_nbarrels--; }

void StudentWorld::update_gold_count(void) { m_frackman->update_gold(1); }

void StudentWorld::update_sonar_count(void) { m_frackman->update_sonar(1); }

void StudentWorld::update_water_count(void) { m_frackman->update_water(5); }

void StudentWorld::illuminate_goodies(void) {
  for (int i = 0; i < m_actors.size(); i++) {
    if ((m_actors[i]->get_id() == IID_GOLD || m_actors[i]->get_id() == IID_BARREL) &&
        (radius_from_actor(m_actors[i]->get_x(), m_actors[i]->get_y(), 12.00, false, true))) { m_actors[i]->set_visible(true); }
  }
  
  return;
}

void StudentWorld::set_bribe(int x, int y) { new Gold(x, y, this, 1, true); }

void StudentWorld::set_squirt(int x, int y, GraphObject::Direction dir) { new WaterSquirt(x, y, this, dir); }

///////////////////////////////////////////////////////////////////////////
////////////-----------STUDENTWORLD DIRT FUNCTIONS-------------////////////
///////////////////////////////////////////////////////////////////////////

bool StudentWorld::remove_dirt(Actor* a) {
  bool did_remove = false;
  for (int i = a->get_x(); i < a->get_x() + 4; i++) {
    for (int j = a->get_y(); j < a->get_y() + 4; j++) {
      if (m_dirt[i][j] != nullptr) {
        delete m_dirt[i][j];
        m_dirt[i][j] = nullptr;
        
        // Set return status
        did_remove = true;
      }
    }
  }
  
  return did_remove;
}

bool StudentWorld::is_dirt(Actor* a, GraphObject::Direction start_dir) {
  // Get actor's coordinates
  int x = a->get_x();
  int y = a->get_y();
  
  int dir = start_dir;
  bool is_dirt = false;
  switch (dir) {
    case GraphObject::left:
      for (int i = y; i < y + 4; i++) { if (m_dirt[x - 1][i] != nullptr) { is_dirt = true; } }
      break;
    case GraphObject::right:
      for (int i = y; i < y + 4; i++) { if (m_dirt[x + 4][i] != nullptr) { is_dirt = true; } }
      break;
    case GraphObject::down:
      for (int i = x; i < x + 4; i++) { if (m_dirt[i][y - 1] != nullptr) { is_dirt = true; } }
      break;
    case GraphObject::up:
      for (int i = x; i < x + 4; i++) { if (m_dirt[i][y + 4] != nullptr) { is_dirt = true; } }
      break;
    default:
      break;
  }
  
  return is_dirt;
}

bool StudentWorld::is_dirt(int x, int y) const {
  if (m_dirt[x][y] != nullptr) { return true; }
  return false;
}

bool StudentWorld::is_dirt_in_square(int x, int y) {
  bool is_dirt = false;
  
  for (int i = x; i < x + 4; i++) {
    for (int j = y; j < y + 4; j++) {
      if (m_dirt[i][j] != nullptr) { is_dirt = true; }
    }
  }
  
  return is_dirt;
}

///////////////////////////////////////////////////////////////////////////
//////////-----------STUDENTWORLD BOULDER FUNCTIONS-------------///////////
///////////////////////////////////////////////////////////////////////////

bool StudentWorld::is_boulder(int x, int y) const {
  for (int i = 0; i < m_actors.size(); i++) {
    if (m_actors[i]->get_id() != IID_BOULDER) { continue; }
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        if ((x == m_actors[i]->get_x() + j) && (y == m_actors[i]->get_y() + k)) { return true; }
      }
    }
  }
  return false;
}

bool StudentWorld::boulder_hit_human(Actor* a) {
  // Get Boulder and Frackman coordinates (need to update to protesters as well)
  int x = a->get_x();
  int y = a->get_y();
  int frack_x = m_frackman->get_x();
  int frack_y = m_frackman->get_y();
  if ((frack_y + 3 == y) && (frack_x >= (x - 3) && frack_x <= (x + 3)))  { return true; }
  return false;
}

///////////////////////////////////////////////////////////////////////////
/////////////-----------MATH/MATH HELPER FUNCTIONS-------------////////////
///////////////////////////////////////////////////////////////////////////

int StudentWorld::radius(int x_1, int y_1, int x_2, int y_2) const { return sqrt(pow((x_2 - x_1), 2) + pow((y_2 - y_1), 2)); }

bool StudentWorld::radius_from_actor(int x, int y, double r, bool is_boulder, bool is_frackman, bool is_protester) const {
  for (int i = 0; i < m_actors.size(); i++) {
    int actor_x = m_actors[i]->get_x();
    int actor_y = m_actors[i]->get_y();
    // Checking radius with a boulder object
    if (is_boulder) {
      if (m_actors[i]->get_id() != IID_BOULDER) { continue; }
    }
    // Checking radius with the frackman player
    if (is_frackman) {
      actor_x = m_frackman->get_x();
      actor_y = m_frackman->get_y();
    }
    // Checking radius with a protester
    if (is_protester) {
      if (m_actors[i]->get_id() != IID_PROTESTER || m_actors[i]->get_id() != IID_HARD_CORE_PROTESTER ) { continue; }
    }
    if (radius(x, y, actor_x, actor_y) <= r) { return true; }
  }
  
  return false;
}

bool StudentWorld::is_out_of_bounds(int x, int y, GraphObject::Direction dir) const {
  switch(dir) {
    case GraphObject::up:
      if (y + 1 > 60) { return true; }
      break;
    case GraphObject::down:
      if (y - 1 < 0) { return true; }
      break;
    case GraphObject::left:
        if (x - 1 < 0) { return true; }
      break;
    case GraphObject::right:
      if (x + 1 > 60) { return true; }
      break;
    case GraphObject::none:
      break;
  }
  
  return false;
}

//Generate a random number (Equation used from Project 1 (no need to reinvent the wheel))
int StudentWorld::rand_int(int min, int max) const {
  if (max < min) { swap(max, min); }
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<int> distro(min, max);
  return distro(generator);
}

void StudentWorld::generate_coordinates(int x_min, int x_max, int y_min, int y_max, int shaft_y_coord,
                                        int* x_coord, int* y_coord, bool is_water) {
  bool regenerate = false;
  do {
    // "Randomly" generate oil barrel position
    *x_coord = rand_int(x_min, x_max);
    *y_coord = rand_int(y_min, y_max);
    // Generating coordinates for a water pool, find a dirt-less location
    if (is_water) {
      if (is_dirt_in_square(*x_coord, *y_coord)) { regenerate = true; }
      else { regenerate = false; }
    }
    // Generating coordinates for an oil barrel, boulder, or gold nugget
    else {
      // If a oil barrel receives coordinates within the mineshaft, then regenerate coordinates
      if (*x_coord >= 27 && *x_coord <= 33 && *y_coord >= shaft_y_coord) { regenerate = true; }
      else { regenerate = false; }
    }
  } while (regenerate);
}

void StudentWorld::annoy_frackman(int how_much) { m_frackman->get_annoyed(how_much); }

///////////////////////////////////////////////////////////////////////////
////////-----------STUDENTWORLD PROTESTER FUNCTIONS-------------///////////
///////////////////////////////////////////////////////////////////////////

bool StudentWorld::is_facing_frackman(Protester* protester) const {
  // Get protester's information
  int protester_x = protester->get_x();
  int protester_y = protester->get_y();
  GraphObject::Direction protester_direction = protester->get_direction();
  // Get frackman's coordinates
  int frackman_x = m_frackman->get_x();
  int frackman_y = m_frackman->get_y();
  
  // Take the protester's location as the origin of a coordinate system
  // Check each of the four quadrants for frackman
  if ((frackman_x >= protester_x) && (frackman_y >= protester_y)) {
    if ((protester_direction == GraphObject::right) || (protester_direction == GraphObject::up)) { return true; }
  }
  if ((frackman_x <= protester_x) && (frackman_y >= protester_y)) {
    if ((protester_direction == GraphObject::left) || (protester_direction == GraphObject::up)) { return true; }
  }
  if ((frackman_x <= protester_x) && (frackman_y <= protester_y)) {
    if ((protester_direction == GraphObject::left) || (protester_direction == GraphObject::down)) { return true; }
  }
  if ((frackman_x >= protester_x) && (frackman_y <= protester_y)) {
    if ((protester_direction == GraphObject::right) || (protester_direction == GraphObject::down)) { return true; }
  }
  
  return false;
}

bool StudentWorld::is_in_line_of_sight(Protester* protester) const {
  if (m_frackman->get_x() == protester->get_x() || m_frackman->get_y() == protester->get_y()) { return true; }
  return false;
}


bool StudentWorld::can_move_to_frackman(Protester* protester) {
  // Get protester's information
  int protester_x = protester->get_x();
  int protester_y = protester->get_y();
  // Get frackman's coordinates
  int frackman_x = m_frackman->get_x();
  int frackman_y = m_frackman->get_y();

  // Can potentially move up or down to frackman
  if (protester_x == frackman_x) {
    // Check if can move up to frackman
    if (protester_y < frackman_y) {
      for (int i = protester_x; i < protester_x + 4; i++) {
        for (int j = protester_y; j < frackman_y; j++) {
          if (is_dirt(i, j) || is_boulder(i, j)) {
            return false;
          }
        }
      }
      protester->set_direction(GraphObject::up);
      protester->move_to(protester_x, protester_y + 1);
      return true;
    }
    // Check if can move down to frackman
    if (protester_y > frackman_y) {
      for (int i = protester_x; i < protester_x + 4; i++) {
        for (int j = frackman_y; j < protester_y; j++) {
          if (is_dirt(i, j) || is_boulder(i, j)) {
            return false;
          }
        }
      }
      protester->set_direction(GraphObject::down);
      protester->move_to(protester_x, protester_y - 1);
      return true;
    }
  }
  // Can potentially move left or right to frackman
  if (protester_y == frackman_y) {
    // Check if can move left to frackman
    if (protester_x > frackman_x) {
      for (int i = protester_y; i < protester_y + 4; i++) {
        for (int j = frackman_x; j < protester_x; j++) {
          if (is_dirt(j, i) || is_boulder(j, i)) {
            return false;
          }
        }
      }
      protester->set_direction(GraphObject::left);
      protester->move_to(protester_x - 1, protester_y);
      return true;
    }
    // Check if can move right to frackman
    if (protester_x < frackman_x) {
      for (int i = protester_y; i < protester_y + 4; i++) {
        for (int j = protester_x; j < frackman_x; j++) {
          if (is_dirt(j, i) || is_boulder(j, i)) {
            return false;
          }
        }
      }
      protester->set_direction(GraphObject::right);
      protester->move_to(protester_x + 1, protester_y);
      return true;
    }
  }
  
  return false;
}

GraphObject::Direction StudentWorld::generate_new_direction(Protester* protester) {
  GraphObject::Direction new_dir = GraphObject::none;
  
  int temp_dir = rand_int(0, 3);
  switch (temp_dir) {
    case 0:
      new_dir = GraphObject::up;
      break;
    case 1:
      new_dir = GraphObject::down;
      break;
    case 2:
      new_dir = GraphObject::left;
      break;
    case 3:
      new_dir = GraphObject::right;
      break;
    default:
      new_dir = GraphObject::none;
      break;
  }
  
  return new_dir;
}

bool StudentWorld::can_move_in_new_direction(Protester* protester, GraphObject::Direction dir) const {
  // Get protester information
  int protester_x = protester->get_x();
  int protester_y = protester->get_y();  
  
  
  // Check if there is dirt, boulder, or out of bounds
  switch(dir) {
    case GraphObject::up:
      if (is_out_of_bounds(protester_x, protester_y, dir)) { return false; }
      if (is_boulder(protester_x, protester_y + 4)) { return false; }
      for (int i = 0; i < 4; i++) {
        if (is_dirt(protester_x + i, protester_y + 4)) { return false; }
      }
      break;
    case GraphObject::down:
      if (is_out_of_bounds(protester_x, protester_y, dir)) { return false; }
      if (is_boulder(protester_x, protester_y - 4)) { return false; }
      for (int i = 0; i < 4; i++) {
        if (is_dirt(protester_x + i, protester_y - 1)) { return false; }
      }
      break;
    case GraphObject::left:
      if (is_out_of_bounds(protester_x, protester_y, dir)) { return false; }
      if (is_boulder(protester_x - 4, protester_y)) { return false; }
      for (int i = 0; i < 4; i++) {
        if (is_dirt(protester_x - 1, protester_y + i)) { return false; }
      }
      break;
    case GraphObject::right:
      if (is_out_of_bounds(protester_x, protester_y, dir)) { return false; }
      if (is_boulder(protester_x + 4, protester_y)) { return false; }
      for (int i = 0; i < 4; i++) {
        if (is_dirt(protester_x + 4, protester_y + i)) { return false; }
      }
      break;
    case GraphObject::none:
      break;
  }
  
  return true;
}

///////////////////////////////////////////////////////////////////////////
//////////-----------STUDENTWORLD PRIVATE FUNCTIONS-------------///////////
///////////////////////////////////////////////////////////////////////////

void StudentWorld::init_dirt(void) {
  for (int i = 0; i < GRID_WIDTH; i++) {
    for (int j = 0; j < GRID_HEIGHT; j++) {
      if ((i >= 30 && i <= 33 && j >= 4 && j <= 59) || (j > 59)) {
        m_dirt[i][j] = nullptr;
      }
      else {
        m_dirt[i][j] = new Dirt(i, j, this);
      }
    }
  }
  
  return;
}

void StudentWorld::deinit_dirt(void) {
  for (int i = 0; i < GRID_WIDTH; i++) {
    for (int j = 0; j < GRID_HEIGHT; j++) {
      delete m_dirt[i][j];
      m_dirt[i][j] = nullptr;
    }
  }
}
