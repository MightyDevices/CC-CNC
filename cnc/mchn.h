/*
 * mchn.h
 *
 *  Created on: 09-01-2016
 *      Author: Tomek
 */

#ifndef CNC_MCHN_H_
#define CNC_MCHN_H_

#include <stdint.h>
#include <sys/cb.h>
#include <cnc/config.h>

/* machining axes */
#define MCHN_AXIS_X                         0
#define MCHN_AXIS_Y                         1
#define MCHN_AXIS_Z                         2
#define MCHN_AXIS_A                         3
#define MCHN_AXIS_B                         4
#define MCHN_AXIS_C                         5

/* flags */
/* motion at max speed */
#define MCHN_FLAGS_TRAVERSE					0x0001
/* stop motion when probe is triggered */
#define MCHN_FLAGS_PROBE					0x0002
/* home motion */
#define MCHN_FLAGS_HOMING					0x0004
/* counter clockwise motion */
#define MCHN_FLAGS_CCW                      0x0008
/* inverse feed rate mode */
#define MCHN_FLAGS_IFRM                     0x0010

/* initialize machining function */
int Mchn_Init(void);
/* set feed rate in steps per second */
int Mchn_SetFeedRate(uint32_t fl, int32_t feed_rate);
/* dwell for number of system ticks */
int Mchn_Dwell(uint32_t ticks, cb_t cb);
/* schedule linear motion */
int Mchn_LinearFeed(int32_t dst[MCHN_AXES_NUM], uint32_t fl, cb_t cb);
/* prepare arc feed */
int Mchn_ArcFeed(int32_t dst[MCHN_AXES_NUM], int axis, int32_t ox, int32_t oy,
    uint32_t flags, cb_t cb);

#endif /* CNC_MCHN_H_ */
