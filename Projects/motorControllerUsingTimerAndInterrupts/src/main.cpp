#include <Arduino.h>
#include "../../../Libraries/button.h"
#include "../../../Libraries/pwm.h"
#include "../../../Libraries/led.h"
#include "../../../Libraries/lcd.h"
#include "../../../Libraries/timer.h"

void motor(void);
void motorFrequency_getRecent();

static bool motorStatus = false;
volatile uint16_t count = 0;


void setup() {
	sei();
	button_init(true);
	timer5_start();
	timer5_setCallback(&motorFrequency_getRecent);
	// lcd_init();
	// lcd_clear();
	// lcd_setCursor(0,0);
	// fprintf(lcdout , "Started");
	button_setRotaryButtonCallback(&motor);
	led_redInit();
	led_greenInit();
	led_yellowInit();
	button_setJoystickButtonCallback(&led_redToggle);
	pwm_init();
	EICRA |= (1 << ISC31) | (1 << ISC30) | (1 << ISC21) | (1 << ISC20) | (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00);
	EIMSK |= (1 << INT0);
}

void loop() {
}
	
void motor(){
if(motorStatus == false){  
		motorStatus = true;
		pwm_setDutyCycle(170);   // setting the duty cycle of motor
	}
	else if(motorStatus == true){
		motorStatus = false;
		pwm_setDutyCycle(0);
		led_greenOff();
	}
}


ISR(INT0_vect){   // update the led on encoder value of motor, also increment count to measure speed
	led_yellowToggle();
	count++;
}

void motorFrequency_getRecent(){
	// lcd_clear();
	// lcd_setCursor(0,0);
	// fprintf(lcdout , "Motor Speed = %d" , (count*10));
	count = 0;

}
