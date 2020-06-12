/*
 * game_position.h
 *
 * Author: Peter Sutton
 */ 

#ifndef GAME_POSITION_H_
#define GAME_POSITION_H_

#include <stdint.h>

///////////////////////////////////////////////////////////
// The game is played on a 16x8 display - 16 columns and 8 rows.
// Positions in the game map directly to LED matrix coordinates
// (X - the column number - is 0 to 15, Y (row number) is 0 to 7)
//
// Game positions (x,y) where x (column number) is 0 to 15 and y (row
// number) is 0 to 7 are represented in a single 8 bit unsigned integer
// where the most significant 4 bits are the x value and the least
// significant 4 bits are the y value.

// The following macros allow the extraction of x and y
// values from a combined position value and the construction of a combined
// position value from separate x, y values. Values are assumed to be in
// valid ranges. Invalid positions are any where bit 3 is 1 (i.e. y value
// greater than 7). We can use all 1's (255) to represent an arbitrary
// invalid position.
#define GAME_POSITION(x,y)		( ((x) << 4)|((y) & 0x0F) )
#define GET_X_POSITION(posn)	((posn) >> 4)
#define GET_Y_POSITION(posn)	((posn) & 0x0F)
#define INVALID_POSITION		255

// Return the position that is (deltaX,deltaY) away from the given position.
// deltaX positive is to the right, negative is to the left
// deltaY positive is above, deltaY negative is below
// If the original position is not valid we return INVALID_POSITION.
// If the new position is off the game field, then we return INVALID_POSITION.
uint8_t neighbour_position(uint8_t position, int8_t deltaX, int8_t deltaY);

// Return the position that is to the immediate right of the given position.
// It is assumed that the given position is NOT in the rightmost column.
uint8_t position_to_right_of(uint8_t position);


#endif /* GAME_POSITION_H_ */
