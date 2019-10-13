/*
 * Rotary.c
 *
 *  Created on: 2019
 *      Author: Azher
 */

#include "rotary.h"
#include "lcd.h"
#include "led.h"

#define ROTARY_PORT_A					PORTB	// rotary encoder port 1
#define ROTARY_PORT_B					PORTG	// rotary encoder port 2
#define ROTARY_PIN_A					5
#define ROTARY_PIN_B					2
#define ROTARY_NUM_DEBOUNCE_CHECKS 		5

/*
 * Function pointer declarations
 */

static pTypeRotaryCallback clockwiseCallback = NULL;
static pTypeRotaryCallback counterClockwiseCallback = NULL;

/*
 * Initialization of rotary pins
 */

void rotary_init() {
	DDR_REGISTER(ROTARY_PORT_A) &= (0 << ROTARY_PIN_A); // making pins input
	DDR_REGISTER(ROTARY_PORT_B) &= (0 << ROTARY_PIN_B); // making pins input
	ROTARY_PORT_A |= (1 << ROTARY_PIN_A);		// Enabling internal pullup
	ROTARY_PORT_B |= (1 << ROTARY_PIN_B);		// Enabling internal pullup
}

/*
 * set callback for clockwise direction of rotator
 */
void rotary_setClockwiseCallback(pTypeRotaryCallback cb) {
	clockwiseCallback = cb;
}

/*
 * set callback for counter clockwise direction of rotator
 */
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback cb) {
	counterClockwiseCallback = cb;
}

/*
 * Callback-procedure to plot the samples of the rotary pins on the LCD after first change
 */
void check_rotary() {
	static uint8_t p = 0;
	static uint8_t state[ROTARY_NUM_DEBOUNCE_CHECKS] = {};
	static uint8_t index = 0;
	typedef enum {idle = 0x03, clockwise = 0x02 , counterclockwise = 0x01 , bet = 0x0} status_t;
	static status_t previousStatus = idle;
	state[index] = 0;
	if(PIN_REGISTER(ROTARY_PORT_A) & (1 << ROTARY_PIN_A)) {
	state[index] |= 0x01;
	}
	if(PIN_REGISTER(ROTARY_PORT_B) & (1 << ROTARY_PIN_B)) {
	state[index] |= 0x02;
	}
	index++;
	if(index == ROTARY_NUM_DEBOUNCE_CHECKS){
		index = 0;
		uint8_t j = 0xFF;
		for(uint8_t i = 0; i < ROTARY_NUM_DEBOUNCE_CHECKS; i++) {
			j = j & state[i];
		}
	if(j == clockwise && previousStatus == idle){
		if(clockwiseCallback != NULL)clockwiseCallback();
	}
	else if(j == counterclockwise && previousStatus == idle){
		if(counterClockwiseCallback != NULL)counterClockwiseCallback();
	}
//	if(j != idle){previousStatus = j;}
	previousStatus = j;
	}
}
