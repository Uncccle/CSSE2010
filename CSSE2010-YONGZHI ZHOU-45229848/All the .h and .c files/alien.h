/*
 * alien.h
 *
 *  Author: Peter Sutton
 */ 

#ifndef ALIEN_H_
#define ALIEN_H_

// Aliens occupy 2x2 pixels. We record the position of the bottom left pixel.
// (See game_position.h for how positions are recorded.)
// Aliens can not overlap with each other, or the background

// The maximum number of aliens that can appear at any one time is 5.
#define MAX_ALIENS 5

// Initialise alien data (no aliens to start with)
void init_aliens(void);

// Attempt to move an alien. A random alien will be moved in a random direction
// - up, down or left - if possible. Note that the player may be dead after this
// move if the alien moves into the player. Does nothing if there are no aliens.
void move_random_alien(void);

// Add an alien to the game (if possible). The alien will only be added on the
// right hand side - if there is space to do so. The alien number is returned, or
// -1 will be returned if none can be added. Note that the player may be dead
// after this if the alien is placed on top of the player. 
int8_t add_alien_to_game(void);

// Return the alien at the given position - or -1 if there is no alien at that
// position. (This checks all pixels of the alien, not just the bottom left pixel.)
int8_t alien_at(uint8_t position);

// Return 1 if there is an alien at the given position, 0 otherwise
uint8_t is_alien_at(uint8_t position);

// Indicate that a projectile has hit the given alien at the given position. If
// the alien's energy is exhausted, the alien will be removed.
void alien_hit_at(uint8_t alien_number, uint8_t projectile_position);

// Check aliens prior to background scroll. If there is background to the 
// immediate right of any alien, then attempt to move the alien to the left.
// If this isn't possible (e.g. alien or background in the way) then remove
// the alien. This ensures that aliens never overlap with the background.
void check_aliens_prior_to_background_scroll(void);


#endif /* ALIEN_H_ */