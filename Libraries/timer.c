/* INCLUDES ******************************************************************/
#include "timer.h"
#include "led.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC    1249
#define TIMER2_CYC_FOR_1MILLISEC	249

static pTimerCallback timer1_Callback = NULL;
static pTimerCallback timer2_Callback = NULL;
static pTimerCallback timer5_Callback = NULL;
/*FUNCTION DEFINITION ********************************************************/

void timer2_setCallback(pTimerCallback cb) {
	if (cb != NULL){
		timer2_Callback = cb;
	}

}

/* 8-bit timer/counter with PWM and Asynchronous operation*/
void timer2_start() {
	TCCR2A |= (1 << WGM21); 				// Configure timer for CTC mode
	TCCR2B |= (1 << CS22); 					// Start timer at F_CPU/64
	TIMSK2 |= (1 << OCIE2A); 				// Enable compare match A interrupt
	TCNT2   = 0;							// Initializing counter value
	OCR2A   = TIMER2_CYC_FOR_1MILLISEC; 	// Set CTC compare value with a prescalar of 64
}

void timer2_stop() {
	TCCR2B &= (0 << CS22);					// Stopping the timer with no clocking source
}


void timer5_start(){
//	TCCR5A // Nothing to change in TCCR5A
	/*Set CTC mode and clock pre-scalar of 64 */
	TCCR5B |= (1 << WGM52) | (1 << CS51) | (1 << CS50);
	/*Initializing timer with 0*/
	TCNT5 = 0x0000;
	OCR5A = 249999;
	TIMSK5 |= (1 << OCIE5A);
}

void timer5_stop(){
	TCCR5B &= (0<<CS51) | (0<<CS50);
}

void timer5_setCallback(pTimerCallback cb) {
	if (cb != NULL){
			timer5_Callback = cb;
		}
	}


void timer1_setCallback(pTimerCallback cb) {
	if (cb != NULL){
		timer1_Callback = cb;
	}
}


/* 16-bit timer/counter*/
void timer1_start() {
	TCCR1B |= (1<<WGM12) | (1<<CS11) | (1<<CS10); // Enabling CTC and clock prescalar of 64
	OCR1A = TIMER1_CYC_FOR_5MILLISEC;			  // Set CTC compare value with a prescalar of 64
	TCNT1 = 0;									  // Initializing counter value
	TIMSK1 = (1<<OCIE1A);						  // Enable compare match A interrupt
}

void timer1_stop() {
	TCCR1B &= (0<<CS11) | (0<<CS10);
}

ISR(TIMER1_COMPA_vect) {
	if(timer1_Callback != NULL) timer1_Callback();
}

ISR(TIMER2_COMPA_vect) {
	if(timer2_Callback != NULL) timer2_Callback();
}

ISR(TIMER5_COMPA_vect) {
	if(timer5_Callback != NULL) timer5_Callback();
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
