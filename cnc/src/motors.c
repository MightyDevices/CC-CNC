/*
 * motors.c
 *
 *  Created on: 01-01-2016
 *      Author: Tomek
 */

#include <dev/drv8824.h>
#include <sys/err.h>
#include <stdint.h>

/* initialize stepper motor controller */
int Motors_Init(void)
{
	/* report status */
	return EOK;
}

/* enable/disable stepper motors */
int Motors_Cmd(int enable)
{
	return DRV8824_MotorsCmd(enable);
}

/* perform a single step in all motors according to bit fields */
int Motors_Step(uint32_t step, uint32_t dir)
{
	/* x axis is negated in this machine */
	dir ^= 1 << CNC_AXIS_X;
	/* move motors */
	return DRV8824_Step(step, dir);
}

/* reset step signals */
void Motors_StepReset(void)
{
	/* reset signals */
	DRV8824_StepReset();
}

