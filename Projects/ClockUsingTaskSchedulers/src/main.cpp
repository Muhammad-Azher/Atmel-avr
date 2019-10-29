#include <Arduino.h>
#include "../../../Libraries/scheduler.h"
#include "../../../Libraries/lcd.h"
#include "util/atomic.h"
#include "../../../Libraries/button.h"
#include "../../../Libraries/led.h"

#define HOUR_DIVIDERFORMILLISEC  3600000
#define MIN_DIVIDERFORMILLISEC   60000
#define SEC_DIVIDERFORMILLISEC   1000

static uint32_t setTime_t = 0;

enum setClock{  // for getting the current update status, i.e. what we are changing
	setHour = 0x01,
	setMin = 0x02,
	startClk = 0x03,
	running = 0x04
};


struct time_t {  // struc for keep track of time
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t milli;
	};

struct time_t getTime;
static enum setClock clockStatus = setHour; // initialize with setting hour

taskDescriptor time_taskDescriptor;
taskDescriptor button_taskDescriptor;

void update_alarmClockStatus();
void update_alarmClockTime();
void print_time();

void setup() {
  scheduler_init();
	lcd_init();
	lcd_setCursor(0,0);
	button_init(true);
	led_redInit();
	led_yellowInit();
	button_setJoystickButtonCallback(&update_alarmClockStatus);
	button_setRotaryButtonCallback(&update_alarmClockTime);
	time_taskDescriptor.task = &print_time;
	time_taskDescriptor.param = NULL;
	time_taskDescriptor.expire = 1;
	time_taskDescriptor.period = 100;   // after every 100 milli seconds, print the updated time on LCD/Serial
	button_taskDescriptor.task = &button_checkState;
	button_taskDescriptor.param = NULL;
	button_taskDescriptor.expire = 5;
	button_taskDescriptor.period = 5;   // check the state of button pressed with debouncing
	scheduler_add(&time_taskDescriptor);
	scheduler_add(&button_taskDescriptor);
  scheduler_run();
}

void loop() {
}

/*
 * convert the time in readable format and
 * prints on the lcd
 */

void print_time(){
	static uint32_t temp , t = 0;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
	temp = scheduler_getTime();
	getTime.hour = temp/HOUR_DIVIDERFORMILLISEC;
	if(getTime.hour>0){
		temp = temp - (getTime.hour*HOUR_DIVIDERFORMILLISEC);
	}
	getTime.minute = temp/MIN_DIVIDERFORMILLISEC;
	if(getTime.minute>0){
			temp = temp - (getTime.minute*MIN_DIVIDERFORMILLISEC);
		}
	getTime.second = temp/SEC_DIVIDERFORMILLISEC;
	if(getTime.second>0){
			temp = temp - (getTime.second*SEC_DIVIDERFORMILLISEC);
		}
	lcd_setCursor(0,0);
	fprintf(lcdout , "%02d:%02d:%02d" , getTime.hour , getTime.minute , getTime.second);
	}

/*
 *  For displaying time set value
 *  */

	t = setTime_t;
	getTime.hour = t/HOUR_DIVIDERFORMILLISEC;
	if(getTime.hour>0){
		t = t - (getTime.hour*HOUR_DIVIDERFORMILLISEC);
	}
	getTime.minute = t/MIN_DIVIDERFORMILLISEC;
	if(getTime.minute>0){
		t = t - (getTime.minute*MIN_DIVIDERFORMILLISEC);
	}
	lcd_setCursor(0,1);
	fprintf(lcdout , "%02d:%02d" , getTime.hour , getTime.minute);
}

/*
 *	Callback function for joystick button
 *	This will update the status of Clock
 */

void update_alarmClockStatus(){
	switch (clockStatus) // update the status of clock
  {
  case setHour:
    clockStatus = setMin;
    break;
  case setMin:
    clockStatus = startClk;
    scheduler_setTime(setTime_t);
    break;  
  case startClk:
    clockStatus = running;
    break;      
  case running:
    clockStatus = setHour;
    setTime_t = 0;
    break;      
  }
	led_redToggle();
}
/*
 * Call back function for rotary button
 * Use to update the hour and minute of clock
 */
void update_alarmClockTime(){
	if(clockStatus == setHour){
		setTime_t += HOUR_DIVIDERFORMILLISEC;
		if(setTime_t > 82800000)
		{
			setTime_t -= 86400000;
		}
		led_yellowToggle();
	}
	if(clockStatus == setMin){
		setTime_t += MIN_DIVIDERFORMILLISEC;
	}
}

