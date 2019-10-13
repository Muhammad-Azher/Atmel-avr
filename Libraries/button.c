/*
 * button.c
 *
 *  Created on:  2019
 *      Author: Azher
 */

#include "button.h"
#include "timer.h"

#define BUTTON_PORT PORTB			 // different buttons are connected on PORTB
#define BUTTON_ROTARY_ENCODER_PIN 6  // rotary encoder
#define BUTTON_JOYSTICK_PIN 7		 // joystick button
#define BUTTON_NUM_DEBOUNCE_CHECKS 5 // debouncing of a button (number of bounce check of button before getting input)

pButtonCallback RotaryButtonCallback;	  // function pointer of callback when rotary encoder pressed
pButtonCallback JoystickButtonCallback;	// function pointer of callback when joystick moved/pressed
static uint8_t BUTTON_PORT_HISTORY = 0xFF; // initially we take all pins high of button port..

void button_init(bool debouncing)
{
	DDR_REGISTER(BUTTON_PORT) &= (0 << BUTTON_ROTARY_ENCODER_PIN) | (0 << BUTTON_JOYSTICK_PIN); // making pins input
	BUTTON_PORT |= (1 << BUTTON_JOYSTICK_PIN) | (1 << BUTTON_ROTARY_ENCODER_PIN);				// Enabling internal pullup
	if (debouncing)
	{ // if debouncing is enabled
		timer1_start();
	}
	else
	{
		PCICR |= (1 << PCIE0);													 // enabling pin change interrupt 0
		PCMSK0 |= (1 << BUTTON_ROTARY_ENCODER_PIN) | (1 << BUTTON_JOYSTICK_PIN); // Masking the bit in vector 0 corresponds to buttons
	}
}

bool button_isJoystickPressed(void)
{
	return (!(PIN_REGISTER(BUTTON_PORT) & (1 << BUTTON_JOYSTICK_PIN)));
}

bool button_isRotaryPressed(void)
{
	return (!(PIN_REGISTER(BUTTON_PORT) & (1 << BUTTON_ROTARY_ENCODER_PIN)));
}

void button_setRotaryButtonCallback(pButtonCallback callback)
{
	RotaryButtonCallback = callback;
}

void button_setJoystickButtonCallback(pButtonCallback callback)
{
	JoystickButtonCallback = callback;
}

void button_checkState()
{
	static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {};
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;
	// each bit in every state byte represents one button
	state[index] = 0;
	if (button_isJoystickPressed())
	{
		state[index] |= 0x01;
	}
	if (button_isRotaryPressed())
	{
		state[index] |= 0x02;
	}
	index++;
	if (index == BUTTON_NUM_DEBOUNCE_CHECKS)
	{
		index = 0;

		// init compare value and compare with ALL reads, only if
		// we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1" in the state
		// array, the button at this position is considered pressed
		uint8_t j = 0xFF;
		for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++)
		{
			j = j & state[i];
		}
		debouncedState = j;
		j = ~lastDebouncedState & debouncedState;
		//	if(debouncedState & (0x01)) JoystickButtonCallback();
		//	if(debouncedState & (0x02)) RotaryButtonCallback();
		if (j & (1 << 0))
		{
			JoystickButtonCallback();
		}
		if (j & (1 << 1))
		{
			RotaryButtonCallback();
		}
		// here we can add more buttons for debouncing
	}
}

/**
 * Interrupt service routine of port on which buttons are connected.
 */

ISR(PCINT0_vect)
{
	uint8_t changedbits; // for checking status of button
	changedbits = PINB ^ BUTTON_PORT_HISTORY;
	BUTTON_PORT_HISTORY = PINB;

	if (changedbits & (1 << BUTTON_ROTARY_ENCODER_PIN))
	{
		RotaryButtonCallback();
	}
	if (changedbits & (1 << BUTTON_JOYSTICK_PIN))
	{
		JoystickButtonCallback();
	}
}
