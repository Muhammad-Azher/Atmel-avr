#ifndef BUTTON_H_
#define BUTTON_H_

/* INCLUDES ******************************************************************/

#include "common.h"


/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder and joystick button
 */
void button_init(bool);

/** 
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);

typedef void (*pButtonCallback)();
void button_setRotaryButtonCallback(pButtonCallback callback);
void button_setJoystickButtonCallback(pButtonCallback callback);
void button_checkState();


#endif /* BUTTON_H_ */

