#ifndef TIMER_H_
#define TIMER_H_

/*INCLUDES *******************************************************************/

#include "common.h"


/*PROTOTYPES *****************************************************************/


/**type of function pointer used as timer callback
 */
typedef void (*pTimerCallback)(void);


/**
 * Sets a function to be called when the timer fires. If NULL is
 * passed, no callback is executed when the timer fires.
 *
 * @param cb  valid pointer to callback function
 */
void timer0_setCallback(pTimerCallback cb);
void timer2_setCallback(pTimerCallback cb);
void timer5_setCallback(pTimerCallback cb);
/**
 * Starts hardware timer 2 of MCU with a period
 * of 1 ms.
 */
void timer2_start();
void timer0_stop();
void timer0_start();

/**
 * Stops timer 2.
 */
void timer2_stop();



/**
 * Sets a function to be called when the timer fires.
 *
 * @param cb  pointer to the callback function; if NULL, no callback
 *            will be executed.
 */
void timer1_setCallback(pTimerCallback cb);


/**
 * Start timer 1 of MCU to trigger on compare match every 5ms.
 */
void timer1_start();


/**
 * Stops timer 1 of the MCU if it is no longer needed.
 */
void timer1_stop();


/**
 * Start timer 5 of MCU to trigger on compare match every 100ms.
 */
void timer5_start();

/**
 * Stops timer 5.
 */
void timer5_stop();



#endif /* TIMER_H_ */


