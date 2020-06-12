/*
 * score.h
 * 
 * Author: Peter Sutton
 */

#ifndef SCORE_H_
#define SCORE_H_

#include <stdint.h>

void init_score(void);
void upgrade_level_player(void);
void display_of_score(void);
void add_to_score(uint16_t value);
void double_speed_in_togglling(void);
void display_of_level(void);
uint8_t get_speeding(void);
uint32_t get_score(void);


#endif /* SCORE_H_ */