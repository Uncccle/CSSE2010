/*
 * score.c
 *
 * Written by Peter Sutton
 */

#include "score.h"
#include <stdio.h>
#include "buttons.h"
#include "pixel_colour.h"
#include "terminalio.h"
#include <util/delay.h>
#include "player.h"
#include "scrolling_char_display.h"


uint32_t score;
uint32_t high_score = 0;
uint8_t Speeding_in_double = 1; 
uint8_t level_at_present;

static const uint8_t color_array[] = {COLOUR_BLACK, COLOUR_RED, COLOUR_YELLOW, COLOUR_GREEN, COLOUR_ORANGE, COLOUR_LIGHT_ORANGE, COLOUR_LIGHT_YELLOW, COLOUR_LIGHT_GREEN};

void init_score(void) {
	score = 0;
	level_at_present = 0;
	display_of_score();
	display_of_level();
	
}

void add_to_score(uint16_t value) {
	if ((score + value * Speeding_in_double) % 100 < score % 100){
		upgrade_level_player();
	}
	score += value;
	if (score > high_score){
		high_score = score;
	} else {
		high_score = high_score;
	}
	display_of_score();
	display_of_level();
}

void double_speed_in_togglling(void){
	if (Speeding_in_double == 1) {
		Speeding_in_double = 2;
	} else if (Speeding_in_double == 2){
		Speeding_in_double = 1;
	}
}




void upgrade_level_player(void) {
	level_at_present++;
	set_scrolling_display_text("LEVEL UP", color_array[level_at_present % 8]);
	
	while (scroll_display()) {
		_delay_ms(150);
		if(button_pushed() == NO_BUTTON_PUSHED) {;}
		}
	_delay_ms(1000);
	redraw_player();

}


void display_of_score(void){
	move_cursor(40, 16);
	printf("%40s", "Score");
	move_cursor(40, 17);
	printf("%40d", (int)score);
	move_cursor(40, 18);
	printf("%40s", "High Score");
	move_cursor(40, 19);
	printf("%40d", (int)high_score);
}

void display_of_level(void){
	move_cursor(40, 20);
	printf("%40s", "level");
	move_cursor(40, 21);
	printf("%40d", (int)level_at_present);
}

uint8_t get_speeding(void){
	return Speeding_in_double;
}

uint32_t get_score(void) {
	return score;
}
