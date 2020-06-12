/*
 * projectile.c
 *
 * Author: Peter Sutton
 *
 */ 

#include "projectile.h"
#include "ledmatrix.h"
#include "pixel_colour.h"
#include "game_position.h"
#include "game_background.h"
#include "alien.h"
#include "player.h"
#include "alien.h"
#include <stdint.h>
#include <stdlib.h>

///////////////////////////////// Global variables //////////////////////
// Projectiles just occupy a single pixel - unlikely there can ever be more than 64
#define MAX_PROJECTILES 64
static uint8_t num_projectiles;
static uint8_t projectile_position[MAX_PROJECTILES];

// Colours
#define COLOUR_PROJECTILE	COLOUR_ORANGE

/////////////////////////////// Function Prototypes for Helper Functions ///////
// These functions are defined after the public functions. Comments are with the
// definitions. These functions are static, so not accessible outside this file.
static void remove_projectile(uint8_t projectile_number);
static void erase_projectile(uint8_t projectile_number);
static void redraw_projectile(uint8_t projectile_number);
		
/////////////////////////////// Public Functions ///////////////////////////////
// These functions are defined in the same order as declared in projectile.h. See
// comments there also.

// Initialise projectile data
void init_projectiles(void) {
	num_projectiles = 0;
}

void fire_projectile_if_possible(void) {
	uint8_t playerX = GET_X_POSITION(get_player_position());
	if(playerX == 14) {
		// Player is at right hand side - can't fire. (Note second pixel of player
		// will be at X=15.)
		return;
	}
	// Determine the position immediately to the right of the player - i.e. 2 pixels
	// right of the left hand pixel of the player
	uint8_t position_to_right_of_player = neighbour_position(get_player_position(), 2, 0);
	if(is_background_at(position_to_right_of_player)) {
		// Background to right - can't fire projectile
		return;
	}
	if(is_projectile_at(position_to_right_of_player)) {
		// There is a projectile to the immediate right of the player - can't fire
		return;
	}
	if(playerX <= 12 && is_projectile_at(position_to_right_of(position_to_right_of_player))) {
		// There is a projectile two pixels to the right of the player - can't fire
		return;
	}
	int8_t alien_num = alien_at(position_to_right_of_player);
	if(alien_num != -1) {
		// There is an alien to the right of the player - we can fire, but we
		// won't see the projectile. Indicate that the alien has been hit.
		alien_hit_at(alien_num, position_to_right_of_player);
	}
	
	// Can fire projectile - add one to the immediate right of the player
	// and draw it on the display
	uint8_t new_projectile_number = num_projectiles;
	num_projectiles++;
	projectile_position[new_projectile_number] = position_to_right_of_player;
	redraw_projectile(new_projectile_number);
}

void advance_projectiles(void) {
	// Remove all the projectiles from the display
	// Iterate over all the projectiles and move them all to the right.
	uint8_t projectile_num = 0;
	while(projectile_num < num_projectiles) {
		// Determine the new position and check what is in that position
		// Note that we generate this new position, but the result is only valid
		// if the projectile is NOT in column 15 - so we check that first and only
		// use this value if that is the case.
		uint8_t new_projectile_posn = position_to_right_of(projectile_position[projectile_num]);
		if(GET_X_POSITION(projectile_position[projectile_num]) == 15) {
			// Projectile is at right hand edge - just remove it
			remove_projectile(projectile_num);
		} else if(is_background_at(new_projectile_posn)) {
			// Projectile has run into background - remove it 
			remove_projectile(projectile_num);
		} else {
			int8_t alien_num = alien_at(new_projectile_posn);
			if(alien_num != -1) {
				// Indicate that we've hit an alien and remove the projectile
				alien_hit_at(alien_num, new_projectile_posn);
				remove_projectile(projectile_num);
			} else if(new_projectile_posn == get_player_position()) {
				// Player is at this position - just remove the projectile
				remove_projectile(projectile_num);
			} else {
				// Nothing at this position - move the projectile forward
				erase_projectile(projectile_num);
				projectile_position[projectile_num] = new_projectile_posn;
				redraw_projectile(projectile_num);
				// Move on to the next projectile number
				projectile_num++;
			
				// Note that in other cases, because we have removed a projectile,
				// this spot in the projectile position array will have been filled
				// with the details of another projectile (unless it was the last one).
				// We don't need to increment the projectile_num.
			}
		}
	}
}

// Return 1 if there is a projectile at the given position, 0 otherwise
uint8_t is_projectile_at(uint8_t position) {
	// Check each projectile
	for(uint8_t i = 0 ; i < num_projectiles; i++) {
		if(projectile_position[i] == position) {
			return 1;
		}
	}
	return 0;
}

// Remove any projectile at the given position.
void remove_any_projectile_at(uint8_t position) {
	// Check each projectile
	for(uint8_t i = 0; i < num_projectiles; i++) {
		if(projectile_position[i] == position) {
			// Found one - remove it and return - no need to check other positions
			remove_projectile(i);
			return;
		}
	}
}

// Remove the projectile from the game. Usually because it has hit something.
static void remove_projectile(uint8_t projectile_number) {
	// Remove the projectile from the display
	erase_projectile(projectile_number);
	// Rearrange our array of projectile positions so that there is no hole
	// in the array. If this projectile was not the last one in the array, we
	// just move the last projectile into this position.
	uint8_t last_projectile_num = num_projectiles - 1;
	if(projectile_number != last_projectile_num) {
		projectile_position[projectile_number] = projectile_position[last_projectile_num];
	}
	// else projectile was the last one in the array
	
	// Shorten our array
	num_projectiles -= 1;
}

//////////////////////// REDRAWING FUNCTIONS /////////////////////////////////

// Erase the given projectile from the display
static void erase_projectile(uint8_t projectile_number) {
	uint8_t x = GET_X_POSITION(projectile_position[projectile_number]);
	uint8_t y = GET_Y_POSITION(projectile_position[projectile_number]);
	
	ledmatrix_update_pixel(x, y, COLOUR_BLACK);
}

// Redraw the given projectile
static void redraw_projectile(uint8_t projectile_number) {
	uint8_t x = GET_X_POSITION(projectile_position[projectile_number]);
	uint8_t y = GET_Y_POSITION(projectile_position[projectile_number]);
		
	ledmatrix_update_pixel(x, y, COLOUR_PROJECTILE);
}