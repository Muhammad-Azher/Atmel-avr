/*
 * pwm.c
 *
 *  Created on: 2019
 *      Author: Azher
 */


#include "pwm.h"
#include "common.h"
#include "lcd.h"


void pwm_init(){
	TCNT0 = 0; // initializing the register with zero value
	lcd_setCursor(0,0);
	fprintf(lcdout , "0");
	PRR0 &= (0 << PRTIM0);
//	TCCR0B &= (0 << CS01); // No pre-scalar
//	TCCR0B &= (0 << CS02); // No pre-scalar
//	TCCR0B = 0x09;
//	TCCR0B |= (1 << CS00); | (1 << WGM02);// We have problem here
	DDRG |= (1 << 5);
	fprintf(lcdout , "1");
	TCCR0A |= (1 << WGM00) | (1 << WGM01) | (1 << COM0B0) | (1 << COM0B1);// For fast PWM mode (Compare at top 0xFF)
	TCCR0B |= (1 << WGM02);
	TCCR0B |= (1 << CS00);
	fprintf(lcdout , "4");


//	TCCR0A |= (1 << COM0B0) | (1 << COM0B1); // Set OC0B on compare match, clear OC0B at bottom
//	TCCR0B |= (1 << WGM02) | (1 << CS00) ;// For fast PWM mode (Compare at top 0xFF)

//	TCCR0B |= (1 << CS00) ;// No pre-scalar

	fprintf(lcdout , "6");

}



void pwm_setDutyCycle(uint8_t dutyCycle){
	OCR0A = dutyCycle;
}


