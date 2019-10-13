/* INCLUDES ******************************************************************/

#include "common.h"
#include "led.h"

/* DEFINES & MACROS **********************************************************/

#define LED_RED_PORT       	PORTG		// port of red led is PORTG		
#define LED_RED_PIN         	1		// red led is at pin1


#define LED_YELLOW_PORT 	PORTF		// port of yellow led is PORTF
#define LED_YELLOW_PIN      	7		// red led is at pin7


#define LED_GREEN_PORT 		PORTF		// port of green led is PORTF		
#define LED_GREEN_PIN       	6		// red led is at pin6


/* FUNCTION DEFINITION *******************************************************/

//Each port pin consists of three register bits: DDxn, PORTxn, and PINxn
//The DDxn bit in the DDRx Register selects the direction of this pin. If DDxn is written logic one, Pxn is
//configured as an output pin. If DDxn is written logic zero, Pxn is configured as an input pin.
//If PORTxn is written logic one when the pin is configured as an input pin, the pull-up resistor is activated. To
//switch the pull-up resistor off, PORTxn has to be written logic zero or the pin has to be configured as an output
//pin. The port pins are tri-stated when reset condition becomes active, even if no clocks are running.
//If PORTxn is written logic one when the pin is configured as an output pin, the port pin is driven high (one). If
//PORTxn is written logic zero when the pin is configured as an output pin, the port pin is driven low (zero).


void led_redInit(void) {
	DDR_REGISTER(LED_RED_PORT) |= (1<<LED_RED_PIN);
}

void led_redToggle(void) {
	LED_RED_PORT ^= (1<<LED_RED_PIN);
}

void led_redOn(void) {
	LED_RED_PORT = (0<<LED_RED_PIN);
}

void led_redOff(void) {
	LED_RED_PORT = (1<<LED_RED_PIN);
}

void led_yellowInit(void) {
	DDR_REGISTER(LED_YELLOW_PORT) |= (1<<LED_YELLOW_PIN);
}

void led_yellowToggle(void) {
	LED_YELLOW_PORT ^= (1<<LED_YELLOW_PIN);
}

void led_yellowOn(void) {

	LED_YELLOW_PORT = (0<<LED_YELLOW_PIN);
}

void led_yellowOff(void) {
	LED_YELLOW_PORT = (1<<LED_YELLOW_PIN);
}

void led_greenInit(void) {
	DDR_REGISTER(LED_GREEN_PORT) |= (1<<LED_GREEN_PIN);
}

void led_greenToggle(void) {
	LED_GREEN_PORT ^= (1<<LED_GREEN_PIN);
}

void led_greenOn(void) {
	LED_GREEN_PORT = (0<<LED_GREEN_PIN);
}

void led_greenOff(void) {
	LED_GREEN_PORT = (1<<LED_GREEN_PIN);
}
