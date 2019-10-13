#ifndef COMMON_H_
#define COMMON_H_

/* INCLUDES ******************************************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* DEFINES & MACROS **********************************************************/

/**pin address of port x
 *
 * example: PING == PIN_REGISTER(PORTG)
 */
#define PIN_REGISTER(x) (*(&(x) - 2))

/**address of data direction register of port x
 *
 * example: DDRG == DDR_REGISTER(PORTG)
 */
#define DDR_REGISTER(x) (*(&(x) - 1))

enum {
	SUCCESS = 0,
	INVALID = 1,
	FAIL = 2
};
typedef uint8_t ses_error_t;

#endif /* COMMON_H_ */
