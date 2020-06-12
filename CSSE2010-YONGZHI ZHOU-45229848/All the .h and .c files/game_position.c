/*
 * game_position.c
 *
 * Author: Peter Sutton
 */ 

#include "game_position.h"

// Return the position which is (deltaX,deltaY) away from the supplied position.
uint8_t neighbour_position(uint8_t position, int8_t deltaX, int8_t deltaY) {
	// If the original position is invalid, we just return an invalid position
	// (An invalid position is any where bit 3 is one.)
	if(position & 0x08) {
		return INVALID_POSITION;
	}
	int8_t x = GET_X_POSITION(position) + deltaX;
	int8_t y = GET_Y_POSITION(position) + deltaY;
	if(x < 0 || x > 15 || y < 0 || y > 7) {
		return INVALID_POSITION;
		} else {
		return GAME_POSITION(x,y);
	}
}

// Return the position immediately to the right of the given position.
// The answer is only valid if the given position is NOT in the rightmost
// column. We add one to the X component of the position (bits 7 to 4).
uint8_t position_to_right_of(uint8_t position) {
	return (position + (1<<4));
}

