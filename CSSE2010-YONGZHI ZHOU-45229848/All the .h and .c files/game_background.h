/*
 * game_background.h
 *
 * Author: Peter Sutton
 */ 

#ifndef GAME_BACKGROUND_H_
#define GAME_BACKGROUND_H_

#include <stdint.h>

// Initialise background data and draw the background (after clearing the display)
void init_background(void);

// Returns 1 if there is background at the given position, 0 otherwise
uint8_t is_background_at(uint8_t position);

// Scroll the background to the left by one position. (Aliens aren't 
// allowed to overlap the background so this may push some aliens 
// along - or just remove an alien if it can't be pushed along.)
// Scrolling the background may cause the player to die if the background
// scrolls into the player.
void scroll_background(void);

#endif /* GAME_BACKGROUND_H_ */