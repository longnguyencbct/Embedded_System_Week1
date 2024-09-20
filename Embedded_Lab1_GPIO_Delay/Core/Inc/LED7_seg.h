/*
 * LED7_seg.h
 *
 *  Created on: Sep 19, 2024
 *      Author: clong
 */

#ifndef INC_LED7_SEG_H_
#define INC_LED7_SEG_H_
#include <stdint.h>

void LED7_init ();
void LED7_Scan ();
void LED7_SetDigit ( int num , int position , uint8_t show_dot );
void LED7_SetColon ( uint8_t status );

void LED_On ( uint8_t index );
void LED_Off ( uint8_t index );

extern uint8_t arrayOfNum[10];
extern uint8_t led7seg[4];

#endif /* INC_LED7_SEG_H_ */
