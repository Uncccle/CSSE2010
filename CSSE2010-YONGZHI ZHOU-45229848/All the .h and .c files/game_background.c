/*
 * game_background.c
 *
 * Author: Peter Sutton
 */ 

#include "ledmatrix.h"
#include "pixel_colour.h"
#include "game_position.h"
#include "game_background.h"
#include "alien.h"
#include "player.h"
#include "projectile.h"
#include <stdint.h>

// Background - 8 bits for each column. A 1 indicates background being
// present, a 0 is empty space. We will scroll through this and
// return to the other end. Bit 0 (LSB) in these patterns will end
// up on the bottom of the display (row 0).
#define NUM_GAME_COLUMNS 32
static uint8_t background_data[NUM_GAME_COLUMNS] = {
	0b00000011,
	0b00000111,
	0b00000111,
	0b00000111,
	0b00000011,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000001,
	0b00000001,
	0b00000011,
	0b00000011,
	0b00000001,
	0b00000001,
	0b00010000,
	0b00011000,
	0b00011000,
	0b00111100,
	0b00111100,
	0b00111000,
	0b00011000,
	0b00010000,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000011,
	0b00000111,
	0b00000111
};

// Which column is at the left of the screen - starting at 0 and counting up
// until NUM_GAME_COLUMNS-1 - we then wrap around.
static uint8_t scroll_position;

// Colours
#define COLOUR_BACKGROUND	COLOUR_GREEN

// Helper functions
static void draw_initial_background(void);
static void draw_background_column(uint8_t column);
static void update_background_display_after_scroll(void);

///////////////////////// PUBLIC FUNCTIONS //////////////////////////////////

// Initialise background data
void init_background(void) {
	scroll_position = 0;
	draw_initial_background();
}

// Return 1 if there is background at the given position, 0 otherwise
uint8_t is_background_at(uint8_t position) {
	uint8_t row = GET_Y_POSITION(position);
	uint8_t column = GET_X_POSITION(position);
	// Work out which column number this is in our background array
	uint8_t background_col_number = (column + scroll_position) % NUM_GAME_COLUMNS;
	uint8_t background_col_data = background_data[background_col_number];
	if(background_col_data & (1<< row)) {
		return 1;
	} else {
		return 0;
	}
}

// Scroll the background to the left by one position.
void scroll_background(void) {
	// Check for any aliens that the background will run into and move
	// or remove them
	check_aliens_prior_to_background_scroll();
	
	// scroll_position indicates which column in the background data is
	// shown at column 0 on the display. We increment this and check if
	// need to wrap around back to the start.
	scroll_position++;
	if(scroll_position == NUM_GAME_COLUMNS) {
		scroll_position = 0;
	}
	
	// Update the display
	update_background_display_after_scroll();
	
	// Check whether the player is dead or not. (Will redraw player if so.)
	check_if_player_is_dead();
}

/////////////////////// STATIC FUNCTIONS /////////////////////////////////////

// Clear the screen and draw the background. The player is not drawn.
void draw_initial_background() {
	// Clear the display
	ledmatrix_clear();
	
	uint8_t column;
	for(column = 0; column <= 15; column++) {
		draw_background_column(column);
	}
}

// Draw the column with the given number (0 to 15). The player is not drawn.
static void draw_background_column(uint8_t column) {
	MatrixColumn column_display_data;
	uint8_t i;
	uint8_t game_column = scroll_position + column;
	uint8_t background_column_data = background_data[game_column % NUM_GAME_COLUMNS];
	for(i=0;i<=7;i++) {
		if(background_column_data & (1<<i)) {
			// Bit i is set, meaning background is present
			column_display_data[i] = COLOUR_BACKGROUND;
		} else {
			column_display_data[i] = COLOUR_BLACK;
		}
	}
	ledmatrix_update_column(column, column_display_data);
}

static void update_background_display_after_scroll(void) {
	// We iterate over our background and erase pixels that 
	// are no longer part of the background - and add those 
	// that now are part of the background. We need to work out 
	// the background column data that was in this column and
	// the background column data that will be in this column.
	for(uint8_t column = 0; column <= 15; column++) {
		int8_t old_game_column = scroll_position + column - 1;
		if(old_game_column < 0) {
			// If the last column of data in the background array has just scrolled
			// off the left hand side, then we need to get the data from the
			// end of the array, not the previous position.
			// (old_game_column should only ever be -1 at this point)
			old_game_column += NUM_GAME_COLUMNS;
		}
		uint8_t new_game_column = scroll_position + column;
		uint8_t old_column_data = background_data[old_game_column % NUM_GAME_COLUMNS];
		uint8_t new_column_data = background_data[new_game_column % NUM_GAME_COLUMNS];
		for(uint8_t row=0; row <= 7; row++) {
			if( (old_column_data & (1<<row)) != (new_column_data & (1<<row)) ) {
				// Pixel has changed
				if(new_column_data & (1<<row)) {
					// Background is now present
					// Remove any projectile that is in this position
					remove_any_projectile_at(GAME_POSITION(column,row));
					ledmatrix_update_pixel(column, row, COLOUR_BACKGROUND);
				} else {
					ledmatrix_update_pixel(column, row, COLOUR_BLACK);
				}
			}
		}
	}
}