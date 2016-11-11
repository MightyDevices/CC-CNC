/*
 * spindle.c
 *
 *  Created on: Oct 27, 2016
 *      Author: Tomek
 */

#include <dev/pwm.h>
#include <cnc/spindle.h>
#include <cnc/config.h>
#include <sys/err.h>

/* current values */
static int on, dir;
/* revolutions per minute */
static fixp_t rpm;

/* set spindle rpms */
int Spindle_SetRpms(fixp_t rpms)
{
	/* sanity check */
	if (rpms < 0 || rpms > fixp(SPINDLE_RPM_MAX))
		return EFATAL;

	/* store information */
	rpm = rpms;
	/* set duty cycle according to rpms */
	return Pwm_SetDutyCycle(rpm * 100 / fixp(SPINDLE_RPM_MAX));
}

/* set spindle on/off */
int Spindle_SetOnOff(int state, int direction)
{
	/* rpms */
	fixp_t _rpm = rpm;
	/* store state */
	on = state; dir = direction;
	/* disable spindle */
	if (state == SPINDLE_OFF)
		_rpm = 0;

	/* set rpms */
	return Pwm_SetDutyCycle(_rpm * 100 / fixp(SPINDLE_RPM_MAX));
}
