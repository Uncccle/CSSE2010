/*
 * player.c
 *
 * Author: Peter Sutton
 */ 

#include "ledmatrix.h"
#include "pixel_colour.h"
#include "game_position.h"
#include "projectile.h"
#include "alien.h"
#include "game_background.h"
#include "terminalio.h"
#include "player.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

///////////////////////////////// Global variables //////////////////////
// player_position stores the current position of the player's left pixel (x,y).
// The player occupies two pixels horizontally so the player also occupies the 
// position to the right of this (x+1,y).
static uint8_t player_position;
static uint8_t player_blood = 4;
#define INITIAL_PLAYER_POSITION		GAME_POSITION(0,4)

// We keep track of whether the player is alive or not. player_dead true if dead.
static uint8_t player_dead;

// Colours
#define COLOUR_PLAYER		COLOUR_YELLOW
#define COLOUR_DEAD_PLAYER	COLOUR_LIGHT_YELLOW	

/////////////////////////////// Function Prototypes for Helper Functions ///////
// These functions are defined after the public functions. Comments are with the
// definitions. These functions are static, so not accessible outside this file.

static void erase_player(void);
void show_blood(void);


/////////////////////////////// Public Functions ///////////////////////////////
// These functions are defined in the same order as declared in player.h. See
// comments there also.

// Reset the player data and draw the player on the display. It is assumed
// that the display has been cleared and the background drawn. 
void init_player(void) {
	// Player is put in its initial position
	player_position = INITIAL_PLAYER_POSITION;
	
	// Player is not initially dead :-)
	player_dead = 0;
	
	// Display the initial player. It is assumed that the
	// initial player position does not overlap the background.
	// We do not check for this.
	redraw_player();
}


///////////////////////////// Movement functions /////////////////////////////
// Note that these may cause the player to die - this should be checked after
// the move is made/attempted.

void show_blood(void){
	move_cursor(30, 21);
	printf("%30s", "Blood");
	move_cursor(30, 22);
	printf("%30d", (int)player_blood);
}

void move_player_up(void) {
	if(GET_Y_POSITION(player_position) != 7) {
		// player not at top - can make the move. Erase the player from
		// the display, update the player's position and redraw the
		// player. If the player runs into a projectile then the projectile is 
		// removed.
		erase_player();
		player_position = GAME_POSITION(GET_X_POSITION(player_position),
				GET_Y_POSITION(player_position) + 1);	// This increments the Y position
		// If either pixel of the player runs into a projectile, then remove
		// the projectile.
		remove_any_projectile_at(player_position);
		remove_any_projectile_at(position_to_right_of(player_position));
		check_if_player_is_dead();
		redraw_player();
	} // else player is at top (row 7) and can't move up
}

void move_player_down(void) {
	if(GET_Y_POSITION(player_position) != 0) {
		// player not at bottom - can make the move. Erase the player from
		// the display, update the player's position and redraw the
		// player. If the player runs into a projectile then the projectile is
		// removed.
		erase_player();
		player_position = GAME_POSITION(GET_X_POSITION(player_position),
		GET_Y_POSITION(player_position) - 1);	// This decrements the Y position
		// If either pixel of the player runs into a projectile, then remove
		// the projectile.
		remove_any_projectile_at(player_position);
		remove_any_projectile_at(position_to_right_of(player_position));
		check_if_player_is_dead();
		redraw_player();
	} // else player is at bottom (row 0) and can't move up
}

void move_player_left(void) {
	if(GET_X_POSITION(player_position) != 0) {
		// player not at LEFT - can make the move. Erase the player from
		// the display, update the player's position and redraw the
		// player. If the player runs into a projectile then the projectile is
		// removed.
		erase_player();
		player_position = GAME_POSITION(GET_X_POSITION(player_position) -1, GET_Y_POSITION(player_position));	// This decrements the X position
		// If either pixel of the player runs into a projectile, then remove
		// the projectile.
		remove_any_projectile_at(player_position);
		remove_any_projectile_at(position_to_right_of(player_position));
		check_if_player_is_dead();
		redraw_player();
	} // else player is at LEFT (row 0) and can't move LEFT
}

void move_player_right(void) {
	if(GET_X_POSITION(player_position) != 14) {
		// player not at right - can make the move. Erase the player from
		// the display, update the player's position and redraw the
		// player. If the player runs into a projectile then the projectile is
		// removed.
		erase_player();
		player_position = GAME_POSITION(GET_X_POSITION(player_position) + 1 ,GET_Y_POSITION(player_position));	// This increments the X position
		// If either pixel of the player runs into a projectile, then remove
		// the projectile.
		remove_any_projectile_at(player_position);
		remove_any_projectile_at(position_to_right_of(player_position));
		check_if_player_is_dead();
		redraw_player();
	} // else player is at right (row 0) and can't move right
}

uint8_t get_player_position(void) {
	return player_position;
}

void check_if_player_is_dead(void) {
	// The position of the second player pixel is immediately to the right -
	// work out this position
	uint8_t second_pixel_position = position_to_right_of(player_position);
		
	if(is_background_at(player_position) || is_background_at(second_pixel_position) ||
			is_alien_at(player_position) || is_alien_at(second_pixel_position)) {
		// Have just worked out that the player is dead - redraw them
		player_dead = 1;
		redraw_player();
	}
}

uint8_t is_player_dead(void) {
	return player_dead;
}

/////////////////////////////// Private (Helper) Functions /////////////////////

//////////////////////// REDRAWING FUNCTIONS /////////////////////////////////

// Erase the player (we assume it hasn't crashed - so we just replace
// the player position with black)
static void erase_player(void) {
	uint8_t playerX = GET_X_POSITION(player_position);
	uint8_t playerY = GET_Y_POSITION(player_position);
	ledmatrix_update_pixel(playerX, playerY, COLOUR_BLACK);
	ledmatrix_update_pixel(playerX + 1, playerY, COLOUR_BLACK);
}

// Redraw the player in its current position.
void redraw_player(void) {
	uint8_t player_colour = COLOUR_PLAYER;
	if(!player_blood){
		player_colour = COLOUR_DEAD_PLAYER;
	}
	uint8_t playerX = GET_X_POSITION(player_position);
	uint8_t playerY = GET_Y_POSITION(player_position);
	ledmatrix_update_pixel(playerX, playerY, player_colour);
	ledmatrix_update_pixel(playerX + 1, playerY, player_colour);
}
