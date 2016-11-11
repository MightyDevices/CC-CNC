/*
 * led.c
 *
 *  Created on: Oct 21, 2016
 *      Author: Tomek
 */

#include <dev/led.h>
#include <sys/err.h>
#include <sys/time.h>

/* timer */
static time_t t;
/* state */
static int state;

/* test leds */
int TestLed_Init(void)
{
	/* return */
	return EOK;
}

/* poll test */
void TestLed_Poll(void)
{
	/* check timer */
	if (dtime(time(), t) < 3500)
		return;

	/* set leds */
	Led_BsyBlink(state ? 20 : 500, state ? 2000 : 1000);
	Led_ErrBlink(state ? 500 : 20, state ? 1000 : 2000);
	/* negate state */
	state = !state;
	/* kick timer */
	t = time();
}
