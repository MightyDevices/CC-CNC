/*
 * limits.h
 *
 *  Created on: Oct 23, 2016
 *      Author: Tomek
 */

#ifndef CNC_LIMITS_H_
#define CNC_LIMITS_H_


/* limit flags */
#define LIM_AN								0x01
#define LIM_AP								0x02
#define LIM_ZM								0x04
#define LIM_ZP								0x08
#define LIM_YM								0x10
#define LIM_YP								0x20
#define LIM_XM								0x40
#define LIM_XP								0x80

/* get limit switches status */
int Limits_GetStatus(void);

#endif /* CNC_LIMITS_H_ */
