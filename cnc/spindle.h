/*
 * spindle.h
 *
 *  Created on: Oct 27, 2016
 *      Author: Tomek
 */

#ifndef CNC_SPINDLE_H_
#define CNC_SPINDLE_H_

#include <util/fixp.h>

/* spindle options */
#define SPINDLE_ON						    1
#define SPINDLE_OFF						    0
#define SPINDLE_CW						    1
#define SPINDLE_CCW						    0

/* set spindle rpms */
int Spindle_SetRpms(fixp_t rpms);
/* set spindle on/off */
int Spindle_SetOnOff(int state, int direction);

#endif /* CNC_SPINDLE_H_ */
