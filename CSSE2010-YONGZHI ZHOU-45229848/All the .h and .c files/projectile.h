/*
 * projectile.h
 *
 * Author: Peter Sutton
 */ 

#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include <stdint.h>

// Initialise projectile data (no projectiles to start with)
void init_projectiles(void);

// Fire a projectile if possible. This will be possible unless there is already
// a projectile in the one or two pixels to the right of the player, or the
// player is at the right hand edge of the game field, or there is background
// to the immediate right.
void fire_projectile_if_possible(void);

// Move all the projectiles forward. If any collide with the background, they
// are removed. If any collide with an alien, they reduce the alien's energy
// (and remove the alien if its energy is 0). If they hit the player, we just
// remove the projectile (and no damage to the player).
void advance_projectiles(void);

// Return 1 if there is a projectile at the given position, 0 otherwise
uint8_t is_projectile_at(uint8_t position);

// Remove any projectile at the given position. No action taken if there is
// no projectile at that position
void remove_any_projectile_at(uint8_t position);

#endif /* PROJECTILE_H_ */