/*
 * player.h
 *
 * Author: Peter Sutton
 */ 


#ifndef PLAYER_H_
#define PLAYER_H_ 

#include <stdint.h>

// Initialise player data and draw player on the display
void init_player(void);
void show_blood(void);
/////////////////////////////////// MOVE FUNCTIONS /////////////////////////
// check_if_player_is_dead() should be checked after calling one of these to see
// if the move succeeded or not. The move may fail if the player
// moves into the background or into an alien. If the player is at the
// edge then no move is made. If the player moves into a projectile, the
// projectile is just removed.

// Attempt to move the player one row up
void move_player_up(void);

// Attempt to move the player one row down
void move_player_down(void);

// Attempt to move the player one column left
void move_player_left(void);

// Attempt to move the player one column right
void move_player_right(void);

// Get the position that the player is currently in. (The player also occupies the column
// immediately to the right.) See game_position.h for details of the position.
uint8_t get_player_position(void);

// Check if the player is dead or not. This will check whether
// the player overlaps with an alien or the background.
// (And update the display of the player to indicate death.)
void check_if_player_is_dead(void);

void redraw_player(void);
// Return true if the player is dead, false otherwise. 
// This function does not check if the player is dead, it only returns true (1)
// if the player is already known to be dead.
uint8_t is_player_dead(void);

#endif /* PLAYER_H_ */