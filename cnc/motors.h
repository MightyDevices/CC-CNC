/*
 * motors.h
 *
 *  Created on: 01-01-2016
 *      Author: Tomek
 */

#ifndef CNC_MOTORS_H_
#define CNC_MOTORS_H_

#include <stdint.h>

/* initialize stepper motor controller */
int Motors_Init(void);
/* perform a single step in all motors according to bit fields */
int Motors_Step(uint32_t step, uint32_t dir);
/* reset step signals */
void Motors_StepReset(void);
/* enable/disable stepper motors */
int Motors_Cmd(int enable);

#endif /* CNC_MOTORS_H_ */
