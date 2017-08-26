/*******************************************************************************
 *Actor.cpp
 *
 * This routine is responsible for all of the objects (actors) within the game
 * The main function of each actor is to do something each clock tick
 *
 * $LastChangedBy: jless $
 * $LastChangedDate: 2017-08-23 11:11:11 -0700 (Wed, 23 Aug 2017) $
 *
 ******************************************************************************/

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------INCLUDES--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <algorithm>
using namespace std;

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------GLOBALS--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------ACTOR--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

Actor::Actor(int image_id, int start_x, int start_y, Direction start_dir, float image_size,
             unsigned int image_depth, StudentWorld* current_world)
: GraphObject(image_id, start_x, start_y, start_dir, image_size, image_depth), m_world(current_world), m_alive(true) {}

bool Actor::is_alive(void) { return m_alive; }

void Actor::set_dead(void) { m_alive = false; set_visible(false); }

StudentWorld* Actor::world(void) { return m_world; }

Actor::~Actor() {}

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------DIRT--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

Dirt::Dirt(int start_x, int start_y, StudentWorld* world)
: Actor(IID_DIRT, start_x, start_y, GraphObject::right, 0.25, 3, world) { set_visible(true); }

void Dirt::do_something() { return; }

Dirt::~Dirt() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
//////////////////////-----------BOULDER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Boulder::Boulder(int start_x, int start_y, StudentWorld* world)
: Actor(IID_BOULDER, start_x, start_y, GraphObject::down, 1.00, 1, world), m_nticks_before_fall(30), m_state(stable)
{ set_visible(true); world->add_actor(this); }

void Boulder::do_something(void) {
  // Check if Boulder is still alive
  if (!is_alive()) { set_dead(); }
  
  // Get current boulder coordinates
  int x = get_x(), y = get_y();
  
  // Get pointer to the StudentWorld
  StudentWorld* boulder_world = world();
  
  // Check state of the boulder
    // If in stable state, if there is no dirt directly below boulder, transition to waiting state
  if (m_state == 0) {
    if (!boulder_world->is_dirt(this, GraphObject::down)) { m_state = 1; }
  }
    // If in waiting state, if 30 ticks have elapsed, transition to falling state (and play sound effect for falling boulder)
  else if (m_state == 1) {
    if (m_nticks_before_fall-- <= 0) { m_state = 2; boulder_world->play_sound(SOUND_FALLING_ROCK); }
  }
    // If in falling state, then move down 1 step each tick, until it hits bottom of oil field, another boulder, dirt, or human
  else if (m_state == 2) {
    // If hits bottom of oil field
    if (y <= 0) { set_dead(); }
    // If hits another boulder
    else if (boulder_world->boulder_hit_actor(this, false, true)) { set_dead(); }
    // If hits dirt
    else if (boulder_world->is_dirt(this, GraphObject::down)) { set_dead(); }
    // If hits frackman
    else if (boulder_world->boulder_hit_actor(this)) { set_dead(); world()->annoy_frackman(100); }
    // If hits a protester 
    else if (boulder_world->boulder_hit_actor(this, false)) { set_dead(); }
    // Else move down one step
    else { move_to(x, y - 1); }
  }
  
  return;
}

Boulder::~Boulder() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
//////////////////-----------WATER SQUIRT--------------////////////////////
///////////////////////////////////////////////////////////////////////////

WaterSquirt::WaterSquirt(int start_x, int start_y, StudentWorld* world, Direction start_dir)
: Actor(IID_WATER_SPURT, start_x, start_y, start_dir, 1.00, 1, world), m_nticks_before_vanish(4)
{ set_visible(true); world->add_actor(this); }

void WaterSquirt::do_something(void) {
  // Get current water squirt coordinates
  int x = get_x(), y = get_y();
  
  // Get pointer to StudentWorld
  StudentWorld* watersquirt_world = world();
  
  // Check if the water squirt has hit a protester
  /// TODO: IF WATER SQUIRT IS WITHIN RADIUS OF 3 OF A PROTESTER, THEN DEAL 2 POINTS OF ANNOYANCE, AND SET STATE TO DEAD
  
  // Check if the water squirt has traveled its complete distance
  if (m_nticks_before_vanish <= 0) { set_dead(); }
  
  // Check if the water squirt can move in the specified direction (i.e. no dirt, no boulders, no out of bounds, no protester)
    // If boulder blocks path
  if (watersquirt_world->radius_from_actor(x, y, 3.00, true)) { set_dead(); }
    // If out of bounds or dirt blocks path (or protester blocks path)
  int direction = get_direction();
  switch(direction) {
    case GraphObject::left:
      if (x <= 0 || watersquirt_world->is_dirt(this, GraphObject::left)) { set_dead(); }
      else if (watersquirt_world->radius_from_actor(x, y, 3.00, false, false, true, false)) { set_dead(); }
      else { move_to(x - 1, y); }
      break;
    case GraphObject::right:
      if (x >= 60 || watersquirt_world->is_dirt(this, GraphObject::right)) { set_dead(); }
      else if (watersquirt_world->radius_from_actor(x, y, 3.00, false, false, true, false)) { set_dead(); }
      else { move_to(x + 1, y); }
      break;
    case GraphObject::down:
      if (y <= 0 || watersquirt_world->is_dirt(this, GraphObject::down)) { set_dead(); }
      else if (watersquirt_world->radius_from_actor(x, y, 3.00, false, false, true, false)) { set_dead(); }
      else { move_to(x, y - 1); }
      break;
    case GraphObject::up:
      if (y >= 60 || watersquirt_world->is_dirt(this, GraphObject::up)) { set_dead(); }
      else if (watersquirt_world->radius_from_actor(x, y, 3.00, false, false, true, false)) { set_dead(); }
      else { move_to(x, y + 1); }
      break;
    default:
      break;
  }  
  
  // Update ticks before water squirt vanishes
  m_nticks_before_vanish--;
  
  return;
}

WaterSquirt::~WaterSquirt() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------HUMAN--------------//////////////////////
///////////////////////////////////////////////////////////////////////////

Human::Human(int image_id, int start_x, int start_y, Direction start_dir, float image_size,
             unsigned int image_depth, StudentWorld* current_world, int start_health)
: Actor(image_id, start_x, start_y, start_dir, image_size, image_depth, current_world), m_health(start_health) {}

void Human::get_annoyed(int how_much) { m_health -= how_much; }

int Human::get_health(void) { return m_health; }

Human::~Human() {}

///////////////////////////////////////////////////////////////////////////
/////////////////////-----------FRACKMAN--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Frackman::Frackman(StudentWorld* world)
: Human(IID_PLAYER, 30, 60, GraphObject::right, 1.00, 0, world, 10), m_squirts(5), m_sonars(1), m_gold(0) { set_visible(true); }

void Frackman::do_something() {
  // Check if Frackman is still alive
  if (!is_alive()) { return; }
  if (get_health() <= 0) { set_dead(); }
  
  // Get frackman's coordinates
  int x = get_x(), y = get_y();
  
  // Get pointer to the StudentWorld
  StudentWorld* frack_world = world();
  
  // Check if in dirt, if so delete the dirt (and play the digging sound effect)
  if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
  
  // Check if player pressed a valid key
  int key;
  if(frack_world->get_key(key)) {
    switch(key) {
      // Aborts current level (i.e. player lost a live, reset level (if lives remaining))
      case KEY_PRESS_ESCAPE:
        get_annoyed(10);
        break;
      // Frackman fires a water squirt from squirt gun
      case KEY_PRESS_SPACE:
        if (get_squirts() >= 1) {
          // Decrement water squirts
          m_squirts--;
          // Play sound effect
          frack_world->play_sound(SOUND_FINISHED_LEVEL);
          // Get direction that frackman is facing
          Direction dir = get_direction();
          int x_coord = x, y_coord = y;
          switch(dir) {
            case GraphObject::left: x_coord -= 3; break;
            case GraphObject::right: x_coord += 3; break;
            case GraphObject::down: y_coord -= 3; break;
            case GraphObject::up: y_coord += 3; break;
            default: break;
          }
          frack_world->set_squirt(x_coord, y_coord, dir);
        }
        break;
      // Frackman places gold nugget down as bribe to a protester
      case KEY_PRESS_TAB:
        // If Frackman has 1 or more gold nuggets, then place a bribe down, and decrement gold count
        if (get_gold() >= 1) { frack_world->set_bribe(x, y); update_gold(-1); }
        break;
      // Frackman uses sonar kit (to illuminate the region around him)
        /*
         ii. All hidden game objects (e.g., Gold Nuggets or Barrels of oil) that
         are within a radius of 12 (e.g., this includes 11.99 squares away)
         must be made visible via setVisible() and revealed to the player.
         */
      case KEY_PRESS_Z:
      case KEY_PRESS_z:
        // If Frackman has 1 or more sonar kits, illuminate contents of oil field (radius of 12), play sound, decrement count
        if (get_sonars() >= 1) {
          frack_world->illuminate_goodies();
          frack_world->play_sound(SOUND_SONAR);
          update_sonar(-1);
        }
        break;
      // Left
      case KEY_PRESS_LEFT:
        // If not currently facing in the direction of key press
        if (get_direction() != GraphObject::left) { set_direction(GraphObject::left); break; }
        // If facing in the direction of key press, check that the movement is valid
        if (x <= 0) {
          // Move in place (per the spec)
          move_to(x, y);
        }
        else {
          // Check if there is a boulder blocking the way
          if (world()->radius_from_actor(x - 1, y, 3.00, true, false, false)) { move_to(x, y); }
          else {
            move_to(x - 1, y);
            if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
          }
        }
        break;
      // Right
      case KEY_PRESS_RIGHT:
        // If not currently facing in the direction of key press
        if (get_direction() != GraphObject::right) { set_direction(GraphObject::right); break; }
        // If facing in the direction of key press, check that the movement is valid
        if (x >= 60) {
          // Move in place (per the spec)
          move_to(x, y);
        }
        else {
          // Check if there is a boulder blocking the way
          if (world()->radius_from_actor(x + 1, y, 3.00, true, false, false)) { move_to(x, y); }
          else {
            move_to(x + 1, y);
            if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
          }
        }
        break;
      // Down
      case KEY_PRESS_DOWN:
        // If not currently facing in the direction of key press
        if (get_direction() != GraphObject::down) { set_direction(GraphObject::down); break; }
        // If facing in the direction of key press, check that the movement is valid
        if (y <= 0) {
          // Move in place (per the spec)
          move_to(x, y);
        }
        else {
          // Check if there is a boulder blocking the way
          if (world()->radius_from_actor(x, y - 1, 3.00, true, false, false)) { move_to(x, y); }
          else {
            move_to(x, y - 1);
            if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
          }
        }
        break;
      // Up
      case KEY_PRESS_UP:
        // If not currently facing in the direction of key press
        if (get_direction() != GraphObject::up) { set_direction(GraphObject::up); break; }
        // If facing in the direction of key press, check that the movement is valid
        if (y >= 60) {
          // Move in place (per the spec)
          move_to(x, y);
        }
        else {
          // Check if there is a boulder blocking the way
          if (world()->radius_from_actor(x, y + 1, 3.00, true, false, false)) { move_to(x, y); }
          else {
            move_to(x, y + 1);
            if (frack_world->remove_dirt(this)) { frack_world->play_sound(SOUND_DIG); }
          }
        }
        break;
      default: break;
    }
  }
  
  return;
}

int Frackman::get_squirts(void) const { return m_squirts; }

int Frackman::get_gold (void) const { return m_gold; }

int Frackman::get_sonars(void) const { return m_sonars; }

void Frackman::update_gold(int how_much) { m_gold += how_much; }

void Frackman::update_sonar(int how_much) { m_sonars += how_much; }

void Frackman::update_water(int how_much) { m_squirts += how_much; }

Frackman::~Frackman() {}

///////////////////////////////////////////////////////////////////////////
////////////////////-----------PROTESTER--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Protester::Protester(StudentWorld* world, int image_id, int start_health)
: Human(image_id, 60, 60, GraphObject::left, 1.0, 0, world, start_health)
{
  set_visible(true);
  world->add_actor(this);
  set_squares_current_direction(world->rand_int(8, 60));
  set_resting_ticks(MAX(0, 3 - world->get_level() / 4));
  m_ticks_since_shouted = 0;
  m_ticks_since_turned = 0;
  m_leave_oil_field_state = false;
}

void Protester::do_something(void) {
  // Check the status of the regular protester
  if (!is_alive()) { return; }
  
  // Check if regular protester can move this clock tick
  if (get_resting_ticks() > 0) { update_resting_ticks(-1); return; }
  
  // Get current coordinates of regular protester
  int x = get_x(), y = get_y();
  
  // Get pointer to StudentWorld
  StudentWorld* protester_world = world();
  
  // Check the leave the oil field state
  if (get_leave_oil_field_state()) {
    // If already at the exit location for regular protester
    if (x == 60 && y == 60) { set_dead(); }
    // If not at the exit, then protester uses Queue-Based Maze-Searching Algorithm to find the exit
    else {
      Direction first = GraphObject::none, second = GraphObject::none, third = GraphObject::none, fourth = GraphObject::none;
      Direction new_dir[4] = {first, second, third, fourth};
      // Generates the priority level for each of the four directions
      protester_world->generate_optimal_direction(this, new_dir[0], new_dir[1], new_dir[2], new_dir[3], true);
      for (int i = 0; i < 4; i++) {
        // If the protester can move in a given direction, then set direction, and move one step in that direction
        if (protester_world->can_move_in_new_direction(x, y, new_dir[i]))
        {
          set_direction(new_dir[i]);
          switch(new_dir[i]) {
            case GraphObject::up: move_to(x, y + 1); break;
            case GraphObject::down: move_to(x, y - 1); break;
            case GraphObject::left: move_to(x - 1, y); break;
            case GraphObject::right: move_to(x + 1, y); break;
            case GraphObject::none: break;
          }
          break;
        }
      }
    }
  }
  // Check if within striking distance of frackman, and facing frackman, and can shout again
  else if (protester_world->radius_from_actor(x, y, 4.00, false, true) && protester_world->is_facing_frackman(this) &&
           get_ticks_since_shouted() <= 0) {
    // Play protester shout sound effect
    protester_world->play_sound(SOUND_PROTESTER_YELL);
    // Inflict 2 points of damage on the frackman
    protester_world->annoy_frackman(2);
    // Reset protester variables
    set_ticks_since_shouted();
    set_resting_ticks(REST_TICKS_SHOUT);
    update_ticks_since_turned(-1);
    return;
  }
  // If a hardcore protester, then implement tracking of frackman
  else if (track_frackman()) { set_resting_ticks(MAX(0, 3 - world()->get_level() / 4)); update_ticks_since_shouted(-1); return; }
  // Check if frackman is in direct line of sight, and not within radius of 4 from frackman, and can actually move to frackman
  else if (protester_world->is_in_line_of_sight(this) && !protester_world->radius_from_actor(x, y, 4.00, false, true) &&
           protester_world->can_move_to_frackman(this)) {
    set_squares_current_direction(0);
    set_resting_ticks(MAX(0, 3 - world()->get_level() / 4));
    update_ticks_since_shouted(-1);
    update_ticks_since_turned(-1);
    return;
  }
  // Else if protester doesn't have direct line of sight of frackman
  else {
    // Update squares to move in current direction
    update_squares_current_direction(-1);
    
    // If number of squares to move in current direction is <= 0, then pick new direction to walk in
    if (get_squares_current_direction() <= 0) {
      bool invalid_direction = false;
      do {
        GraphObject::Direction temp_dir = protester_world->generate_new_direction(this);
        // If protester can't take a single step in the new direction, then pick new direction and recheck
        if (protester_world->can_move_in_new_direction(x, y, temp_dir)) {
          invalid_direction = false;
          set_direction(temp_dir);
        }
        else { invalid_direction = true; }
      } while (invalid_direction);
      // Pick new number for squares to walk in new direction
      set_squares_current_direction(protester_world->rand_int(8, 60));
      update_ticks_since_turned(-1);
    }
    // If can still move in the current direction, check to see if protester can move (i.e. check if there is dirt or boulder blocking it)
    else {
      // If protester hasn't turned for the specified duration, check if it is at an intersection
      if (get_ticks_since_turned() <= 0) {
        // If at an intersection, and can take one step in a given path, choose a path
        bool turn = false, can_move_up = false, can_move_down = false, can_move_left = false, can_move_right = false;
        int temp_dir = -1;
        switch(get_direction()) {
          case GraphObject::up:
          case GraphObject::down:
            // Check if can make perpendicular move in left or right direction, or continue moving up
            if (protester_world->can_move_in_new_direction(x, y, GraphObject::left)) { can_move_left = true; }
            if (protester_world->can_move_in_new_direction(x, y, GraphObject::left)) { can_move_right = true; }
            if (!can_move_left && !can_move_right) { /* Keep moving in current direction */ }
            else if (can_move_left && !can_move_right) { set_direction(GraphObject::left); turn = true; }
            else if (!can_move_left && can_move_right) { set_direction(GraphObject::right); turn = true; }
            else if (can_move_left && can_move_right) {
              temp_dir = protester_world->rand_int(0, 1);
              if (temp_dir == 0) { set_direction(GraphObject::left); turn = true; }
              if (temp_dir == 1) { set_direction(GraphObject::right); turn = true; }
            }
            break;
          case GraphObject::left:
          case GraphObject::right:
            // Check if can make perpendicular move in up or down direction, or continue moving left
            if (protester_world->can_move_in_new_direction(x, y, GraphObject::down)) { can_move_down = true; }
            if (protester_world->can_move_in_new_direction(x, y, GraphObject::up)) { can_move_up = true; }
            if (!can_move_down && !can_move_up) { /* Keep moving in current direction */ }
            else if (can_move_down && !can_move_up) { set_direction(GraphObject::down); turn = true; }
            else if (!can_move_down && can_move_up) { set_direction(GraphObject::up); turn = true; }
            else if (can_move_down && can_move_up) {
              temp_dir = protester_world->rand_int(0, 1);
              if (temp_dir == 0) { set_direction(GraphObject::down); turn = true; }
              if (temp_dir == 1) { set_direction(GraphObject::up); turn = true; }
            }
            break;
            case GraphObject::none:
              break;
        }
        
        // If protester did turn, then update protester variables
        if (turn) { set_ticks_since_turned(); set_squares_current_direction(protester_world->rand_int(8, 60)); }
      }
      
      // If protester can move, then do the move
      if (protester_world->can_move_in_new_direction(x, y, get_direction())) {
        switch (get_direction()) {
          case GraphObject::up: move_to(x, y + 1); break;
          case GraphObject::down: move_to(x, y - 1); break;
          case GraphObject::left: move_to(x - 1, y); break;
          case GraphObject::right: move_to(x + 1, y); break;
          case GraphObject::none: break;
        }
      }
      // Else set the steps to move in current direction to 0, so protester can choose a new direction
      else { set_squares_current_direction(0); update_ticks_since_turned(-1); }
    }
  }
  
  // Update protester tick variables
  set_resting_ticks(MAX(0, 3 - world()->get_level() / 4));
  update_ticks_since_shouted(-1);
  update_ticks_since_turned(-1);
  
  return;
}

void Protester::set_squares_current_direction(int how_much) { m_steps_current_direction = how_much; }

void Protester::set_resting_ticks(int how_much) { m_restingticks = how_much; }

void Protester::set_ticks_since_shouted(void) { m_ticks_since_shouted = 15; }

void Protester::set_ticks_since_turned(void) { m_ticks_since_turned = 65; }

void Protester::set_leave_oil_field_state(void) { m_leave_oil_field_state = true; }

void Protester::update_squares_current_direction(int how_much) { m_steps_current_direction += how_much; }

void Protester::update_resting_ticks(int how_much) { m_restingticks += how_much; }

void Protester::update_ticks_since_shouted(int how_much) { m_ticks_since_shouted += how_much; }

void Protester::update_ticks_since_turned(int how_much) { m_ticks_since_turned += how_much; }

int Protester::get_squares_current_direction(void) const { return m_steps_current_direction; }

int Protester::get_resting_ticks(void) const { return m_restingticks; }

int Protester::get_nonresting_ticks(void) const { return m_nonresting_ticks; }

int Protester::get_ticks_since_shouted(void) const { return m_ticks_since_shouted; }

int Protester::get_ticks_since_turned(void) const { return m_ticks_since_turned; }

bool Protester::get_leave_oil_field_state(void) const { return m_leave_oil_field_state; }

bool Protester::track_frackman(void) { return false; }

Protester::~Protester() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
///////////////-----------HARDCORE PROTESTER--------------/////////////////
///////////////////////////////////////////////////////////////////////////

HardcoreProtester::HardcoreProtester(StudentWorld* world)
: Protester(world, IID_HARD_CORE_PROTESTER, 20) {}

bool HardcoreProtester::track_frackman(void) {
  // Get current coordinates of hardcore protester
  int x = get_x();
  int y = get_y();
  
  // Get pointer to StudentWorld
  StudentWorld* hardcore_world = world();
  
  if (!(hardcore_world->radius_from_actor(x, y, 4.00, false, true))) {
    int M = 16 + hardcore_world->get_level() * 2;
    if (hardcore_world->getSquaresFromFrackMan(this) < M) {
      Direction first = GraphObject::none, second = GraphObject::none, third = GraphObject::none, fourth = GraphObject::none;
      Direction new_dir[4] = {first, second, third, fourth};
      // Generates the priority level for each of the four directions
      hardcore_world->generate_optimal_direction(this, new_dir[0], new_dir[1], new_dir[2], new_dir[3], false);
      for (int i = 0; i < 4; i++) {
        // If the protester can move in a given direction, then set direction, and move one step in that direction
        if (hardcore_world->can_move_in_new_direction(x, y, new_dir[i]))
        {
          set_direction(new_dir[i]);
          switch(new_dir[i]) {
            case GraphObject::up: move_to(x, y + 1); break;
            case GraphObject::down: move_to(x, y - 1); break;
            case GraphObject::left: move_to(x - 1, y); break;
            case GraphObject::right: move_to(x + 1, y); break;
            case GraphObject::none: break;
          }
          return true;
        }
      }
    }
  }
  
  return false;
}

HardcoreProtester::~HardcoreProtester() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOODIE--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Goodie::Goodie(int image_id, int start_x, int start_y, Direction start_dir, float image_size,
               unsigned int image_depth, StudentWorld* world, int ticks)
: Actor(image_id, start_x, start_y, start_dir, image_size, image_depth, world), m_nticks_before_vanish(ticks) {}

int Goodie::get_remaining_ticks(void) const { return m_nticks_before_vanish; }

void Goodie::set_remaining_ticks(void) { m_nticks_before_vanish = MAX(100, 300 - 10 * world()->get_level()); }

void Goodie::update_ticks(void) { m_nticks_before_vanish--; }

Goodie::~Goodie() {}

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------BARREL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Barrel::Barrel(int start_x, int start_y, StudentWorld* world)
: Goodie(IID_BARREL, start_x, start_y, GraphObject::right, 1.00, 2, world, 0) { set_visible(false); world->add_actor(this); }

void Barrel::do_something(void) {
  // Check the status of the oil barrel
  if (!is_alive()) { return; }
  
  // Get current coordinates of oil barrel
  int x = get_x(), y = get_y();
  
  // Get pointer to StudentWorld
  StudentWorld* barrel_world = world();
  
  // If currently invisible, and Frackman is nearby to oil barrel, then set visible (and immediately return)
  if (!is_visible() && barrel_world->radius_from_actor(x, y, 4.00, false, true, false)) { set_visible(true); return; }
  
  // If currently visible, and Frackman grabs the oil barrel, then set dead, play sound, and increase score
  if (is_visible() && barrel_world->radius_from_actor(x, y, 3.00, false, true, false)) {
    set_dead();
    barrel_world->play_sound(SOUND_FOUND_OIL);
    barrel_world->increase_score(1000);
    barrel_world->dec_barrels();
  }
  
  return;
}

Barrel::~Barrel() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
///////////////////////-----------GOLD--------------///////////////////////
///////////////////////////////////////////////////////////////////////////

Gold::Gold(int start_x, int start_y, StudentWorld* world, int state, bool is_visible)
: Goodie(IID_GOLD, start_x, start_y, GraphObject::right, 1.00, 2, world, 100), m_state(state)
{ set_visible(is_visible); world->add_actor(this); }

void Gold::do_something(void) {
  // Check the status of the gold object
  if (!is_alive()) { return; }
  
  // Get current coordinates of gold object
  int x = get_x(), y = get_y();
  
  // Get pointer to StudentWorld
  StudentWorld* gold_world = world();
  
  // If in permanent state (i.e. can be picked up by frackman)
  if (is_permanent_state()) {
    // If currently invisible, and Frackman is nearby to gold, then set visible (and immediately return)
    if (!is_visible() && gold_world->radius_from_actor(x, y, 4.00, false, true, false)) { set_visible(true); return; }
    
    // If currently visible, and Frackman grabs the gold object, then set dead, play sound, and increase score
    if (is_visible() && gold_world->radius_from_actor(x, y, 3.00, false, true, false)) {
      set_dead();
      gold_world->play_sound(SOUND_GOT_GOODIE);
      gold_world->increase_score(10);
      gold_world->update_gold_count();
    }
  }
  // If in temporary state (i.e. can be picked up by protesters)
  else {
    // Check the time before the bribe vanishes
    if (get_remaining_ticks() <= 0) { set_dead(); }
    
    // Update time before bribe vanishes
    update_ticks();
    
    // If protester grabs the bribe, then set dead, play sound, and increase score (handled on StudentWorld side)
    if (gold_world->radius_from_actor(x, y, 3.00, false, false, true, true)) { set_dead(); }

  }
  
  return;
}

bool Gold::is_permanent_state(void) { return (m_state == 0); }

void Gold::update_state(void) { m_state = !m_state; }

Gold::~Gold() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
////////////////////-----------SONAR KIT--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

Sonar::Sonar(StudentWorld* world)
: Goodie(IID_SONAR, 0, 60, GraphObject::right, 1.0, 2, world, 100)
{ set_visible(true); set_remaining_ticks(); world->add_actor(this); }

void Sonar::do_something(void) {
  // Check the status of the sonar kit
  if (!is_alive()) { return; }
  
  // Get current sonar kit coordinates
  int x = get_x(), y = get_y();
  
  // Get pointer to StudentWorld
  StudentWorld* sonar_world = world();
  
  // If frackman grabs the sonar kit, set state to dead, play sound effect, increase player score, and update frackman inventory
  if (sonar_world->radius_from_actor(x, y, 3.00, false, true)) {
    set_dead();
    sonar_world->play_sound(SOUND_GOT_GOODIE);
    sonar_world->increase_score(75);
    sonar_world->update_sonar_count();
  }
  
  // Check the time before the sonar kit vanishes
  if (get_remaining_ticks() <= 0) { set_dead(); }
  
  // Update time before sonar kit vanishes
  update_ticks();
  
  return;
}

Sonar::~Sonar() { set_visible(false); }

///////////////////////////////////////////////////////////////////////////
///////////////////-----------WATER POOL--------------/////////////////////
///////////////////////////////////////////////////////////////////////////

WaterPool::WaterPool(int start_x, int start_y, StudentWorld* world)
: Goodie(IID_WATER_POOL, start_x, start_y, GraphObject::right, 1.0, 2, world, 100)
{ set_visible(true); set_remaining_ticks(); world->add_actor(this); }

void WaterPool::do_something(void) {
  // Check the status of the water pool
  if (!is_alive()) { return; }
  
  // Get current water pool coordinates
  int x = get_x(), y = get_y();
  
  // Get pointer to StudentWorld
  StudentWorld* sonar_world = world();
  
  // If frackman grabs the water pool, set state to dead, play sound effect, increase player score, and update frackman inventory (by 5)
  if (sonar_world->radius_from_actor(x, y, 3.00, false, true)) {
    set_dead();
    sonar_world->play_sound(SOUND_GOT_GOODIE);
    sonar_world->increase_score(100);
    sonar_world->update_water_count();
  }
  
  // Check the time before the water pool vanishes
  if (get_remaining_ticks() <= 0) { set_dead(); }
  
  // Update time before sonar kit vanishes
  update_ticks();
  
  return;
}

WaterPool::~WaterPool() { set_visible(false); }

/* End of Actors */
