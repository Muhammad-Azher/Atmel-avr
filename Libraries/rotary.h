/*
 * rotary.h
 *
 *  Created on: 2019
 *      Author: Azher
 */

#ifndef ROTARY_H_
#define ROTARY_H_

#include "common.h"

/*
 * Function pointer for rotary callback functions
 */
typedef void (*pTypeRotaryCallback)();

/*
 * Initialization of rotary pins
 */
void rotary_init();

/*
 * set callback for clockwise direction of rotator
 */
void rotary_setClockwiseCallback(pTypeRotaryCallback);

/*
 * set callback for counter clockwise direction of rotator
 */
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback);


void check_rotary();



#endif /* SES_ROTARY_H_ */
