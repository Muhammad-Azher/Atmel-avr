#include <Arduino.h>
#include "../../../Libraries/lcd.h"
#include "../../../Libraries/led.h"
#include "../../../Libraries/button.h"
#include "../../../Libraries/adc.h"
#include "../../../Libraries/timer.h"
#include "../../../Libraries/scheduler.h"
#include <util/delay.h>
#include "util/atomic.h"



typedef enum {
	red,
	yellow,
	green
}ledColor;

ledColor color;
int i=0;
static uint8_t msec=0;
static uint8_t sec=0;
static uint8_t min=0;
static uint8_t hr=0;

taskDescriptor led;      // task for blinking led at 0.5/sec
taskDescriptor buttondebounce;    // task for button debounce check
taskDescriptor stopwatch;		  // task for controlling stopwatch
taskDescriptor yellowled;         // for controlling yellow led

static bool stopwatch_Status = false; // current stopwatch status
static bool led_Status = false; // current led status

void stopwatch_edit();
void stopwatch_time();
void led_edit();



void setup() {
  /* Initialization ********************************************************************/
	adc_init();
	lcd_init();
	button_init(true);
	led_redInit();
	led_yellowInit();
	led_greenInit();
	color = green;
	fprintf(lcdout ,"First line");
	lcd_setCursor(0,1);
	fprintf(lcdout ,"Second line");
	_delay_ms(1000);
	lcd_clear();
	i=0;

/* Button task assigning  ************************************************************/
	button_setRotaryButtonCallback(&stopwatch_edit);  // attaching rotary button with stopwatch control
	button_setJoystickButtonCallback(&led_edit);   // attaching joystick button with led control

/* Building tasks for scheduler  ******************************************************/
	if(color == red) led.task = &led_redToggle;
	else if(color == yellow) led.task = &led_yellowToggle;
	else if(color == green) led.task = &led_greenToggle;
	led.param = NULL;
	led.expire = 1;
	led.period = 2000;
	buttondebounce.task = &button_checkState;
	buttondebounce.param = NULL;
	buttondebounce.expire = 5;
	buttondebounce.period = 5;
	stopwatch.task = &stopwatch_time;
	stopwatch.param = NULL;
	stopwatch.expire = 1;
	stopwatch.period = 100;
	yellowled.task = &led_edit;
	yellowled.param = NULL;
	yellowled.expire = 5000;
	yellowled.period = 0;

/* Adding tasks to scheduler  ************************************************************/
	scheduler_add(&buttondebounce);
	scheduler_add(&led);
	scheduler_init();
}

void loop() {
}


/**
 * Attached with rotary button, starts and stops stopwatch
 */
void stopwatch_edit(){
//	lcd_setCursor(0,0);
//	fprintf(lcdout , "Address = %p" , &stopwatch);
	if(stopwatch_Status == false) {
	if(scheduler_add(&stopwatch)){
		stopwatch_Status = true;
	}
	}
	else {
		scheduler_remove(&stopwatch);
		stopwatch_Status = false;
	}
}


/**
 * Prints the elapsed time of stopwatch
 */
void stopwatch_time(){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){i++;
	sec = i/10;
	msec = i%10;}
	lcd_clear();
	lcd_setCursor(0,0);
	fprintf(lcdout , "%d.%d seconds \r\n" , sec,msec);
//	fprintf(lcdout , "%d" , i);
}

/**
 *
 */
void led_edit(){
	if(led_Status == false) {
		if(scheduler_add(&yellowled)){
			led_Status = true;
			led_yellowOn();
		}
	}
	else {
		led_yellowOff();
		led_Status = false;
	}
}
