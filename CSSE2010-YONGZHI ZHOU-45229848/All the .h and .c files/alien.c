/*
 * alien.c
 *
 * Author: Peter Sutton
 */ 

#include "ledmatrix.h"
#include "pixel_colour.h"
#include "game_position.h"
#include "game_background.h"
#include "player.h"
#include "score.h"
#include "projectile.h"
#include "alien.h"
#include <stdint.h>
#include <stdlib.h>


///////////////////////////////// Global variables //////////////////////

// Aliens occupy 2x2 pixels. We record the position of the bottom
// left pixel of each alien (x,y). The alien will also occupy
// (x,y+1), (x+1,y), (x+1,y+1). There can be up to MAX_ALIENS aliens - for each
// alien from 0 up to num_aliens-1, we record the position of that
// alien in the alien_position array. 

static uint8_t num_aliens;
static uint8_t alien_position[MAX_ALIENS];

// Each alien starts with a certain amount of "energy" - once this 
// is depleted the alien is removed from the display. We keep track 
// of the energy for each alien.
static int8_t alien_energy[MAX_ALIENS];
#define INITIAL_ALIEN_ENERGY 10

// Colours
#define COLOUR_ALIEN		COLOUR_RED	
#define COLOUR_ALIEN_HIT	COLOUR_LIGHT_ORANGE

/////////////////////////////// Function Prototypes for Helper Functions ///////
// These functions are defined after the public functions. Comments are with the
// definitions. These functions are static, so not accessible outside this file.

static uint8_t move_alien_left_if_possible(uint8_t alien_number);
static uint8_t move_alien_down_if_possible(uint8_t alien_number);
static uint8_t move_alien_up_if_possible(uint8_t alien_number);
static void move_alien(uint8_t alien_number, uint8_t new_position);
static void remove_alien(uint8_t alien_number);
static void erase_alien(uint8_t alien_number);
static void redraw_alien(uint8_t alien_number);
static void draw_alien_hit(uint8_t position);
		
/////////////////////////////// Public Functions ///////////////////////////////
// These functions are defined in the same order as declared in alien.h. See
// comments there also.

// Initialise alien data
void init_aliens(void) {
	num_aliens = 0;
}

void move_random_alien(void) {
	if(num_aliens == 0) {
		// No aliens - can't move any
		return;
	}
	// Variable to keep track of the number of the alien that we've moved - set 
	// to -1 initially to indicate that we haven't moved an alien.
	int8_t alien_moved = -1;
	
	// Pick the alien that we'll start from. (If we can't move this one
	// we'll move on to the next - and wrap around if necessary until
	// we've tried them all.)
	uint8_t random_initial_alien = rand()%num_aliens;
	uint8_t alien_to_try = random_initial_alien;
	do {
		// Pick a random direction. 0 means left, 1 means up, 2 means down.
		// If we can't move in this direction, we'll try the others
		uint8_t random_direction = rand()%3;
		uint8_t move_made = 0;
		for(uint8_t j = random_direction; j < 3 + random_direction; j++) {
			switch(j%3) {
				case 0:
					move_made = move_alien_left_if_possible(alien_to_try);
					break;
				case 1:
					move_made = move_alien_up_if_possible(alien_to_try);
					break;
				case 2:
					move_made = move_alien_down_if_possible(alien_to_try);
					break;
			}
			if(move_made) {
				// No need to try any other directions - break out of for loop
				break;
			}
		}
		if(move_made) {
			alien_moved = alien_to_try;
			// No need to try any other aliens - break out of do-while loop
			break;
		}
		alien_to_try = (alien_to_try + 1) % num_aliens;
		// Keep going while we haven't tried them all (we'll have broken out of here if
		// we made a move)
	} while(alien_to_try != random_initial_alien);
	
	if(alien_moved != -1) {
		// We have moved an alien - check whether it has collided with player
		// (The move itself has already checked whether the alien collided 
		// with a projectile.)
		check_if_player_is_dead();
	}
}

int8_t add_alien_to_game(void) {
	if(num_aliens == MAX_ALIENS) {
		// Can't add any more - max number already on the display
		return -1;
	}
	// The new alien number will be the current number of aliens.
	uint8_t new_alien_number = num_aliens;
	
	// Alien will be added on the right hand side - the left side of the
	// 2-pixel wide alien will be in the third last column, and the right
	// hand side will be in the second last column.
	// We try row numbers (for the bottom row of the alien) from 
	// 0 to 6 until we find a position where the alien can be inserted 
	// without colliding with the background or another alien
	uint8_t alienX = 13; // Third last column
	for(uint8_t alienY = 0; alienY <= 6; alienY++) {
		// Work out all the positions that would be occupied by the alien
		// if it were placed at (alienX, alienY)
		uint8_t bottom_left = GAME_POSITION(alienX, alienY);
		uint8_t bottom_right = GAME_POSITION(alienX + 1, alienY);
		uint8_t top_left = GAME_POSITION(alienX, alienY + 1);
		uint8_t top_right = GAME_POSITION(alienX + 1, alienY + 1);
		// Check whether any of these positions are occupied by background or 
		// another alien. If not - add an alien
		if(!is_background_at(bottom_left) && !is_background_at(bottom_right) &&
				!is_background_at(top_left) && !is_background_at(top_right) &&
				!is_alien_at(bottom_left) && !is_alien_at(bottom_right) &&
				!is_alien_at(top_left) && !is_alien_at(top_right)) {
			// Nothing at this position - we have found a position for our alien
			// Initialise the alien details, display it, check whether
			// it ended up on top of the player and return.
			alien_position[new_alien_number] = bottom_left;
			alien_energy[new_alien_number] = INITIAL_ALIEN_ENERGY;
			num_aliens++;
			redraw_alien(new_alien_number);
			// Check whether it has collided with the player (which will redraw
			// the player if so)
			check_if_player_is_dead();
			return new_alien_number;
		}
	}
	return -1; // We weren't able to add an alien
}

// Returns the alien number if there is an alien at the given position,
// otherwise it returns -1
int8_t alien_at(uint8_t position) {
	// Check each alien
	for(uint8_t alien_num = 0; alien_num < num_aliens; alien_num++) {
		// We need to check whether any pixel of the alien matches this position
		// so we iterate over all the pixels
		for(uint8_t deltaX = 0; deltaX <= 1; deltaX++) {
			for(uint8_t deltaY = 0; deltaY <= 1; deltaY++) {
				uint8_t position_to_check = neighbour_position(alien_position[alien_num], deltaX, deltaY);
				if(position == position_to_check) {
					// One of the pixels of this alien matches this position
					return alien_num;
				}
			}
		}
	}
	return -1;
}

// Return 1 if there is an alien at the given position, 0 otherwise
uint8_t is_alien_at(uint8_t position) {
	if(alien_at(position) == -1) {
		return 0;
	} else {
		return 1;
	}
}

// Indicate that the given alien has been hit by a projectile at the given position
void alien_hit_at(uint8_t alien_num, uint8_t projectile_position) {
	// Remove the projectile
	remove_any_projectile_at(projectile_position);
	// Decrement the alien's energy
	alien_energy[alien_num]--;
	add_to_score(1);
	if(alien_energy[alien_num] <= 0) {
		add_to_score(5);
		// alien is out of energy
		remove_alien(alien_num);
	} else {
		// alien still has energy - indicate the hit (which will be overwritten
		// when the alien is redrawn, i.e. when it moves)
		draw_alien_hit(projectile_position);
	}
}

// Check each alien prior to the background scrolling. See .h file comment
// for more detail.
void check_aliens_prior_to_background_scroll(void) {
	// Check if there is background to the immediate right of any alien, i.e.
	// the background will scroll into the alien. If so, attempt to move the alien
	// left. If this isn't possible, then remove the alien.
	for(uint8_t i = 0; i < num_aliens; i++) {
		uint8_t alienX = GET_X_POSITION(alien_position[i]);
		uint8_t alienY = GET_Y_POSITION(alien_position[i]);
		if(is_background_at(GAME_POSITION(alienX + 2, alienY)) ||
				is_background_at(GAME_POSITION(alienX + 2, alienY + 1))) {
			// Background will collide with this alien - try moving it left
			if(!move_alien_left_if_possible(i)) {
				// We couldn't move the alien left - remove it
				remove_alien(i);
				// Note - this will change the alien array so that if this alien
				// wasn't the last alien, there will be a new alien in its place.
				// We decrement i so we can check this alien number again.
				i--;
			}
		}
	}
}


// Attempt to move the alien left if possible - returns 1 if it is possible.
// Returns 0 if another alien is in the way OR there is background in the way
// If the alien is in the left most column then the alien is removed.
static uint8_t move_alien_left_if_possible(uint8_t alien_number) {
	uint8_t bottom_left_alien_posn = alien_position[alien_number];
	uint8_t alienX = GET_X_POSITION(bottom_left_alien_posn);
	uint8_t alienY = GET_Y_POSITION(bottom_left_alien_posn);
	if(alienX == 0) {
		// Alien is in left most column - remove the alien
		remove_alien(alien_number);
		return 1;
	} else {
		alienX--; // Proposed new X position
		if(is_background_at(GAME_POSITION(alienX, alienY)) ||
				is_background_at(GAME_POSITION(alienX, alienY+1))) {
			// Can't move left because there is background in the way
			return 0;
		} else if(is_alien_at(GAME_POSITION(alienX, alienY)) ||
				is_alien_at(GAME_POSITION(alienX, alienY+1))) {
			// Can't move left because there is another alien in the way
			return 0;			
		}
		// Nothing in the way - so move.
		move_alien(alien_number, GAME_POSITION(alienX, alienY));
		return 1;
	}
}

// Attempt to move the alien down if possible - returns 1 if it is possible.
// Returns 0 if another alien is in the way OR there is background in the way
// OR the alien is at the bottom of the display.
static uint8_t move_alien_down_if_possible(uint8_t alien_number) {
	uint8_t bottom_left_alien_posn = alien_position[alien_number];
	uint8_t alienX = GET_X_POSITION(bottom_left_alien_posn);
	uint8_t alienY = GET_Y_POSITION(bottom_left_alien_posn);
	if(alienY == 0) {
		// Alien is at the bottom - can't move
		return 0;
	} else {
		alienY--; // Proposed new Y position
		if(is_background_at(GAME_POSITION(alienX, alienY)) ||
				is_background_at(GAME_POSITION(alienX+1, alienY))) {
				// Can't move down because there is background in the way
			return 0;
		} else if(is_alien_at(GAME_POSITION(alienX, alienY)) ||
				is_alien_at(GAME_POSITION(alienX+1, alienY))) {
			// Can't move down because there is another alien in the way
			return 0;
		}
		// Nothing in the way - so move.
		move_alien(alien_number, GAME_POSITION(alienX, alienY));
		return 1;
	}
}

// Attempt to move the alien up if possible - returns 1 if it is possible.
// Returns 0 if another alien is in the way OR there is background in the way
// OR the alien is at the top of the display.
static uint8_t move_alien_up_if_possible(uint8_t alien_number) {
	uint8_t bottom_left_alien_posn = alien_position[alien_number];
	uint8_t alienX = GET_X_POSITION(bottom_left_alien_posn);
	uint8_t alienY = GET_Y_POSITION(bottom_left_alien_posn);
	if(alienY == 6) {
		// Alien is at the top - can't move
		return 0;
	} else {
		alienY++; // Proposed new Y position
		if(is_background_at(GAME_POSITION(alienX, alienY+1)) ||
				is_background_at(GAME_POSITION(alienX+1, alienY+1))) {
			// Can't move up because there is background in the way
			return 0;
		} else if(is_alien_at(GAME_POSITION(alienX, alienY+1)) ||
				is_alien_at(GAME_POSITION(alienX+1, alienY+1))) {
			// Can't move up because there is another alien in the way
			return 0;
		}
		// Nothing in the way - so move. 
		move_alien(alien_number, GAME_POSITION(alienX, alienY));
		return 1;
	}
}

// Helper function used by the move functions above. Move the alien to the 
// given position. The move is known to be OK.
// Erase the alien, update the position and redraw the alien
static void move_alien(uint8_t alien_number, uint8_t new_position) {
	erase_alien(alien_number);
	alien_position[alien_number] = new_position;
	redraw_alien(alien_number);
	
	// Check whether alien has collided with a projectile (could be more than
	// one). new_position is bottom left position
	uint8_t top_left_posn = neighbour_position(new_position, 0, 1);
	uint8_t top_right_posn = position_to_right_of(top_left_posn);
	uint8_t bottom_right_posn = position_to_right_of(new_position);
		
	if(is_projectile_at(new_position)) {
		alien_hit_at(alien_number, new_position);
	}
	// Note that if a hit happens, the alien may be removed - we need to check
	// that it hasn't been removed before doing later checks for projectile 
	// hits. The easiest way is to check that the position of
	// this alien number is still the same as new_position. If it is not, then
	// it means the alien was removed from the list of aliens.
	if(alien_position[alien_number] == new_position && is_projectile_at(top_left_posn)) {
		alien_hit_at(alien_number, top_left_posn);
	}
	if(alien_position[alien_number] == new_position && is_projectile_at(top_right_posn)) {
		alien_hit_at(alien_number, top_right_posn);
	}
	if(alien_position[alien_number] == new_position && is_projectile_at(bottom_right_posn)) {
		alien_hit_at(alien_number, bottom_right_posn);
	}
	
	// Now check whether the alien has collided with the player and redraw the player. 
	// (Note that it is possible for an alien to move into both a player and a projectile
	// in the same move. If this happens to be the projectile hit that destroys the alien
	// then this will have happened before we check whether the player will die.)
	// If the player dies, we redraw it. (We have already redrawn the alien.)
	check_if_player_is_dead();
}

// Remove the alien from the game. (This will happen if it moves off the left
// hand side of the game field, or if it is destroyed by the player.)
static void remove_alien(uint8_t alien_number) {
	// Remove the alien from the display
	erase_alien(alien_number);
	// Rearrange our arrays of alien positions and energies so that there are 
	// no "holes" in them. If this alien wasn't the last in the array, we just
	// move the last alien into this position.
	uint8_t last_alien = num_aliens - 1;
	if(alien_number != last_alien) {
		// This is not the last alien - move the last one into this position
		alien_position[alien_number] = alien_position[last_alien];
		alien_energy[alien_number] = alien_energy[last_alien];
	}
	// else alien was the last one in the arrays.
	
	// Put an invalid position in the entry in the array that we no longer need
	alien_position[last_alien] = INVALID_POSITION;
	
	// Shorten our array
	num_aliens -= 1;
}

//////////////////////// REDRAWING FUNCTIONS /////////////////////////////////

// Erase the given alien - we replace the positions with black
static void erase_alien(uint8_t alien_number) {
	uint8_t alienX = GET_X_POSITION(alien_position[alien_number]);
	uint8_t alienY = GET_Y_POSITION(alien_position[alien_number]);
	
	ledmatrix_update_pixel(alienX, alienY, COLOUR_BLACK);
	ledmatrix_update_pixel(alienX, alienY + 1, COLOUR_BLACK);
	ledmatrix_update_pixel(alienX + 1, alienY, COLOUR_BLACK);
	ledmatrix_update_pixel(alienX + 1, alienY + 1, COLOUR_BLACK);
}

// Redraw the given alien in its current position.
static void redraw_alien(uint8_t alien_number) {
	uint8_t alienX = GET_X_POSITION(alien_position[alien_number]);
	uint8_t alienY = GET_Y_POSITION(alien_position[alien_number]);
	
	ledmatrix_update_pixel(alienX, alienY, COLOUR_ALIEN);
	ledmatrix_update_pixel(alienX, alienY + 1, COLOUR_ALIEN);
	ledmatrix_update_pixel(alienX + 1, alienY, COLOUR_ALIEN);
	ledmatrix_update_pixel(alienX + 1, alienY + 1, COLOUR_ALIEN);
}

// Indicate part of an alien has been hit. (Only lasts until the alien is redrawn, i.e.
// when the background scrolls or the alien moves.)
static void draw_alien_hit(uint8_t position) {
	uint8_t x = GET_X_POSITION(position);
	uint8_t y = GET_Y_POSITION(position);
	ledmatrix_update_pixel(x, y, COLOUR_ALIEN_HIT);
}