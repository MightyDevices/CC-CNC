/*
 * drv8824.h
 *
 *  Created on: Aug 28, 2016
 *      Author: Tomek
 */

#ifndef DEV_DRV8824_H_
#define DEV_DRV8824_H_

#include <stdint.h>
#include <cnc/cnc.h>

/* axis mapping */
#define DRV8824_X							(1 << CNC_AXIS_X)
#define DRV8824_Y							(1 << CNC_AXIS_Y)
#define DRV8824_Z							(1 << CNC_AXIS_Z)
#define DRV8824_A							(1 << CNC_AXIS_A)

/* initialize stepper motor drivers */
int DRV8824_Init(void);
/* enable/disable motors power */
int DRV8824_MotorsCmd(int enable);
/* perform a single step */
int DRV8824_Step(uint32_t step, uint32_t dir);
/* reset step outputs (needed before next step) */
void DRV8824_StepReset(void);

#endif /* DEV_DRV8824_H_ */
