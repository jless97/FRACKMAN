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
  
  // TESTING: initialize boulder
  m_boulder = new Boulder(35, 20, this);
  
  /// TODO: change
  m_nbarrels = 1;
  
  return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
  // Update scoreboard
  update_scoreboard();
  
  // Give frackman a chance to do something
  m_frackman->do_something();
  
  // TESTING: move boulder
  m_boulder->do_something();
  
  // If frackman died during this tick, decrement lives, play sound effect, and if out of lives (GameWorld goes to game over screen)
  if (!m_frackman->is_alive()) {
    play_sound(SOUND_PLAYER_GIVE_UP);
    dec_lives();
    return GWSTATUS_PLAYER_DIED;
  }
  
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
  
  
  return;
}

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

bool StudentWorld::is_dirt_below(Actor* a) {
  bool dirt_below = false;
  for (int i = a->get_x(); i < a->get_x() + 4; i++) {
    if (m_dirt[i][a->get_y() - 1] != nullptr) {
      dirt_below = true;
    }
  }
  
  return dirt_below;
}

bool StudentWorld::boulder_hit_human(Actor* a) {
  if ((a->get_y() <= (m_frackman->get_y() + 4)) && (a->get_x() == m_frackman->get_x())) { return true; }
  return false;
}

bool StudentWorld::radius_from_boulder(int x, int y) const {
  int boulder_x = m_boulder->get_x();
  int boulder_y = m_boulder->get_y();
  double radius = sqrt(pow((boulder_x - x), 2) + pow((boulder_y - y), 2));
  if (radius <= 3) { return true; }
  return false;
}

void StudentWorld::annoy_frackman(int how_much) { m_frackman->get_annoyed(how_much); }

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






