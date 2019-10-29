#include <Arduino.h>
#include "../../../Libraries/scheduler.h"
#include "../../../Libraries/lcd.h"
#include "util/atomic.h"
#include "../../../Libraries/button.h"
#include "../../../Libraries/led.h"
#include "../../../Libraries/rotary.h"

#define HOUR_DIVIDERFORMILLISEC  3600000
#define MIN_DIVIDERFORMILLISEC   60000
#define SEC_DIVIDERFORMILLISEC   1000

#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)

typedef struct fsm_s Fsm; //< typedef for alarm clock state machine

//< event type for alarm clock fsm //
typedef struct event_s {
	uint8_t signal; //< identifies the type of event
} Event;

struct time_t {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t milli;
};

struct time_t getTime;
static uint32_t setAlarmTime_t = 0;

typedef uint8_t fsmReturnStatus; //< typedef to be used with above enum
/** typedef for state event handler functions */
typedef fsmReturnStatus (*State)(Fsm *, const Event*);

typedef struct fsm_s {
	State state;			//< current state, pointer to event handler
	bool isAlarmEnabled; 	//< flag for the alarm status
	struct time_t timeSet; 	//< multi-purpose var for system time and alarm time
} Fsm;

/*
 * Return status of clock
 */
enum {
	ENTRY,
	EXIT,
	setHourAlarm,
	setMinAlarm,
	setHourClock,
	setMinClock,
	clockRunning,
	alarmStatusChange
};

/*
 * Return info which button is pressed
 */
enum press{
	JOYSTICK_PRESSED,
	ROTARY_PRESSED,
	CLOCKWISE_ROTATION,
	COUNTER_CLOCKWISE_ROTATION
} buttonPressed;

/** return values */
enum {
	RET_HANDLED, //< event was handled
	RET_IGNORED, //< event was ignored; not used in this implementation
	RET_TRANSITION //< event was handled and a state transition occurred
};

//
//static uint32_t setTime_t = 0;
//
//enum setClock{
//	setHour = 0x01,
//	setMin = 0x02,
//	startClk = 0x03,
//	running = 0x04
//};

struct time_t alarmTime;
static Fsm alarmClock;
//static enum setClock clockStatus = 0x01;

taskDescriptor time_taskDescriptor;
taskDescriptor button_taskDescriptor;
taskDescriptor rotary_taskDescriptor;
taskDescriptor alarm_taskDescriptor;

/*
 *  FUNCTION DECLARATIONS
 */
void clockInit(Fsm * fsm, const Event * evnt);
void setHourTime_clock(Fsm * fsm, const Event * evnt);
void setMinTime_clock(Fsm * fsm, const Event * evnt);
void clock_Running(Fsm * fsm, const Event * evnt);
void setHourTime_alarm(Fsm * fsm, const Event * evnt);
void setMinTime_alarm(Fsm * fsm, const Event * evnt);
void alarmStateChange(Fsm * fsm, const Event * evnt);
void callBackJoystick();
void callBackRotary();
void callBackClockwiseRotary();
void callBackCounterClockwiseRotary();
void compareTime_alarm();
void update_alarmClockStatus();
void update_alarmClockTime();
void print_time();
inline static void fsm_init(Fsm* fsm, State init);
/***************************************************/

void setup() {
  lcd_init();
	lcd_clear();
	_delay_ms(1000);
	scheduler_init();
	button_init(true);
	led_redInit();
	led_yellowInit();
	led_greenInit();
	lcd_setCursor(0, 0);
	fprintf(lcdout, "Initializing");
	time_taskDescriptor.task = &print_time;
	time_taskDescriptor.param = NULL;
	time_taskDescriptor.expire = 1;
	time_taskDescriptor.period = 100;
	button_setJoystickButtonCallback(&callBackJoystick);
	button_setRotaryButtonCallback(callBackRotary);
	button_taskDescriptor.task = &button_checkState;
	button_taskDescriptor.param = NULL;
	button_taskDescriptor.expire = 5;
	button_taskDescriptor.period = 5;
	rotary_setClockwiseCallback(&callBackClockwiseRotary);
	rotary_setCounterClockwiseCallback(&callBackCounterClockwiseRotary);
	rotary_taskDescriptor.task = &check_rotary;
	rotary_taskDescriptor.param = NULL;
	rotary_taskDescriptor.expire = 5;
	rotary_taskDescriptor.period = 2;
	alarm_taskDescriptor.task = &led_redToggle;
	alarm_taskDescriptor.param = NULL;
	alarm_taskDescriptor.expire = 1;
	alarm_taskDescriptor.period = 250;
	scheduler_add(&rotary_taskDescriptor);
	scheduler_add(&button_taskDescriptor);
	fsm_init((Fsm*) &alarmClock, clockInit);
  scheduler_run();
}

void loop() {
}

/*
 * convert the time in readable format and
 * prints on the lcd
 */

void print_time() {
	static uint32_t temp, t = 0;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		temp = scheduler_getTime();
		getTime.hour = temp / HOUR_DIVIDERFORMILLISEC;
		if (getTime.hour > 0) {
			temp = temp - (getTime.hour * HOUR_DIVIDERFORMILLISEC);
		}
		getTime.minute = temp / MIN_DIVIDERFORMILLISEC;
		if (getTime.minute > 0) {
			temp = temp - (getTime.minute * MIN_DIVIDERFORMILLISEC);
		}
		getTime.second = temp / SEC_DIVIDERFORMILLISEC;
		if (getTime.second > 0) {
			temp = temp - (getTime.second * SEC_DIVIDERFORMILLISEC);
		}
		lcd_setCursor(0, 0);
		fprintf(lcdout, "%02d:%02d:%02d", getTime.hour, getTime.minute,
				getTime.second);
	}
	compareTime_alarm();
}

/* dispatches events to state machine, called in application*/
inline static void fsm_dispatch(Fsm* fsm, const Event* event) {
	static Event entryEvent = { .signal = ENTRY };
	static Event exitEvent = { .signal = EXIT };
	State s = fsm->state;
	fsmReturnStatus r = fsm->state(fsm, event);
	if (r == RET_TRANSITION) {
		s(fsm, &exitEvent); //< call exit action of last state
		fsm->state(fsm, &entryEvent); //< call entry action of new state
	}
}

/* sets and calls initial state of state machine */
inline static void fsm_init(Fsm* fsm, State init) {
//... other initialization
	Event entryEvent = { .signal = ENTRY };
	fsm->state = init;
	fsm->state(fsm, &entryEvent);
}

static void joystickPressedDispatch(void * param) {
	Fsm * fsm = (Fsm *) param;
	Event e = { .signal = JOYSTICK_PRESSED };
	fsm_dispatch(fsm, &e);
}

static void joystickClockwiseRotateDispatch(void * param) {
	Fsm * fsm = (Fsm *) param;
	Event e = { .signal = CLOCKWISE_ROTATION };
	fsm_dispatch(fsm, &e);
}

static void joystickCounterClockwiseRotateDispatch(void * param) {
	Fsm * fsm = (Fsm *) param;
	Event e = { .signal = COUNTER_CLOCKWISE_ROTATION };
	fsm_dispatch(fsm, &e);
}
static void rotaryPressedDispatch(void * param) {
	Fsm * fsm = (Fsm *) param;
	Event e = { .signal = ROTARY_PRESSED };
	fsm_dispatch(fsm, &e);
}

fsmReturnStatus running(Fsm * fsm, const Event* event) {
	switch (event->signal) {
	default:
		return RET_IGNORED;
	}
}

/*
 * FUNCTION DEFINITONS
 */
void clockInit(Fsm * fsm, const Event * evnt) {
	fsm->isAlarmEnabled = false;
	lcd_clear();
	lcd_setCursor(0, 0);
	fprintf(lcdout, "HH:MM");
	switch (evnt->signal) {
	case ROTARY_PRESSED:
		fsm->state = &setHourTime_clock;
		lcd_clear();
		lcd_setCursor(0, 0);
		fprintf(lcdout, "00:00");
	}
}

void setHourTime_clock(Fsm * fsm, const Event * evnt) {
	switch (evnt->signal) {
	case CLOCKWISE_ROTATION:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			fsm->timeSet.hour += 1;
			if (fsm->timeSet.hour > 23) {
				fsm->timeSet.hour = 0;
			}
			lcd_setCursor(0, 0);
			fprintf(lcdout, "%02d:%02d", fsm->timeSet.hour,
					fsm->timeSet.minute);
		}
		break;
	case COUNTER_CLOCKWISE_ROTATION:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			fsm->timeSet.hour -= 1;
			if (fsm->timeSet.hour > 23) {
				fsm->timeSet.hour = 0;
			}
			lcd_setCursor(0, 0);
			fprintf(lcdout, "%02d:%02d", fsm->timeSet.hour,
					fsm->timeSet.minute);
		}
		break;
	case JOYSTICK_PRESSED:
		fsm->state = &setMinTime_clock;
		break;
	}
}

void setMinTime_clock(Fsm * fsm, const Event * evnt) {
	switch (evnt->signal) {
	case CLOCKWISE_ROTATION:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			fsm->timeSet.minute += 1;
			if (fsm->timeSet.minute > 59) {
				fsm->timeSet.minute = 0;
			}
			lcd_setCursor(3, 0);
			fprintf(lcdout, "%02d", fsm->timeSet.minute);
		}
		break;
	case COUNTER_CLOCKWISE_ROTATION:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			fsm->timeSet.minute -= 1;
			if (fsm->timeSet.minute > 59) {
				fsm->timeSet.minute = 0;
			}
			lcd_setCursor(3, 0);
			fprintf(lcdout, "%02d", fsm->timeSet.minute);
		}
		break;
	case JOYSTICK_PRESSED:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			fsm->state = &clock_Running;
			uint32_t set = fsm->timeSet.hour * HOUR_DIVIDERFORMILLISEC;
			set += fsm->timeSet.minute * MIN_DIVIDERFORMILLISEC;
			scheduler_setTime(set);
			scheduler_add(&time_taskDescriptor);
		}
		break;
	}
}

void clock_Running(Fsm * fsm, const Event * evnt) {
	switch (evnt->signal) {
	case ROTARY_PRESSED:
//			fsm->state = alarmStateChange;
		fsm->isAlarmEnabled = !fsm->isAlarmEnabled;
		lcd_setCursor(0, 2);
		fprintf(lcdout, "Alarm is");
		fsm->isAlarmEnabled ?
				fprintf(lcdout, " enabled") : fprintf(lcdout, " disabled");
		break;
	case JOYSTICK_PRESSED:
		scheduler_remove(&time_taskDescriptor);
		fsm->state = &setHourTime_alarm;
		lcd_clear();
		lcd_setCursor(0, 3);
		fprintf(lcdout, "Set alarm hours");
		break;
	}
}

void setHourTime_alarm(Fsm * fsm, const Event * evnt) {
	switch (evnt->signal) {
	case CLOCKWISE_ROTATION:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			alarmTime.hour += 1;
			if (fsm->timeSet.hour > 23) {
				fsm->timeSet.hour = 0;
			}
			lcd_setCursor(0, 0);
			fprintf(lcdout, "%02d:%02d", alarmTime.hour, alarmTime.minute);
		}
		break;
	case COUNTER_CLOCKWISE_ROTATION:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			alarmTime.hour -= 1;
			if (alarmTime.hour > 23) {
				alarmTime.hour = 0;
			}
			lcd_setCursor(0, 0);
			fprintf(lcdout, "%02d:%02d", alarmTime.hour, alarmTime.minute);
		}
		break;
	case JOYSTICK_PRESSED:
		fsm->state = &setMinTime_alarm;
		lcd_setCursor(0, 3);
		fprintf(lcdout, "Set alarm minutes");
		break;
	}
}

void setMinTime_alarm(Fsm * fsm, const Event * evnt) {
	switch (evnt->signal) {
	case CLOCKWISE_ROTATION:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			alarmTime.minute += 1;
			if (alarmTime.minute > 59) {
				alarmTime.minute = 0;
			}
			lcd_setCursor(0, 0);
			fprintf(lcdout, "%02d:%02d", alarmTime.hour, alarmTime.minute);
		}
		break;
	case COUNTER_CLOCKWISE_ROTATION:
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			alarmTime.minute -= 1;
			if (fsm->timeSet.minute > 59) {
				fsm->timeSet.minute = 0;
			}
			lcd_setCursor(0, 0);
			fprintf(lcdout, "%02d:%02d", alarmTime.hour, alarmTime.minute);
			lcd_setCursor(0, 1);
			fprintf(lcdout, "Set M Alarm");
		}
		break;

	case JOYSTICK_PRESSED:
		fsm->state = &clock_Running;
		setAlarmTime_t = alarmTime.hour * HOUR_DIVIDERFORMILLISEC;
		setAlarmTime_t += alarmTime.minute * MIN_DIVIDERFORMILLISEC;
		lcd_clear();
		scheduler_add(&time_taskDescriptor);
		break;
	}
}

void callBackJoystick() {
	joystickPressedDispatch(&alarmClock);
}
void callBackRotary() {
	rotaryPressedDispatch(&alarmClock);
}

void callBackClockwiseRotary() {
	joystickClockwiseRotateDispatch(&alarmClock);
}

void callBackCounterClockwiseRotary() {
	joystickCounterClockwiseRotateDispatch(&alarmClock);
}

void compareTime_alarm() {
	led_yellowToggle();
	if (!alarmClock.isAlarmEnabled)
		return;
	else {
		if((scheduler_getTime() - setAlarmTime_t) >= 0){
		scheduler_add(&alarm_taskDescriptor);
		}
	}
}

