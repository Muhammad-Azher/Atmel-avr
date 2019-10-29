#include <Arduino.h>
#include "../../../Libraries/led.h"
#include "../../../Libraries/button.h"
#include "../../../Libraries/timer.h"
#include <util/delay.h>

volatile uint8_t i = 0;
void softwareTimer(void);

void setup() {
  	/* Initializing all the required components */
	button_init(false);	// Initialize button(passing true will initialize with de-bouncing else initialize PCINT)
	led_redInit();
	led_yellowInit();
	led_greenInit();
	button_setRotaryButtonCallback(&led_yellowOn);
	button_setJoystickButtonCallback(&led_yellowOff);
	timer2_setCallback(&softwareTimer);
	sei();
	timer1_start();    // Timer1 for debounce check
	timer2_start();	 
}

void loop() {
  // put your main code here, to run repeatedly:
}

void softwareTimer(void){  // timer for toggling LED
	i++;
	if(i==100){
		led_redToggle();
		i=0;
	}
}