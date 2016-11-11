/*
 * config.h
 *
 *  Created on: 09-01-2016
 *      Author: Tomek
 */

#ifndef CNC_CONFIG_H_
#define CNC_CONFIG_H_

/* SPINDLE CONFIGURATION */
/* max spindle rpms */
#define SPINDLE_RPM_MAX					    10000


/* MACHINING CONFIGURATION */
/* number of axis */
#define MCHN_AXES_NUM                       6
/* ticks per second */
#define MCHN_TICK_FREQ                      50000
/* steps per millimeters */
#define MCHN_STEPS_PER_MM					400
/* steps per inch */
#define MCHN_STEPS_PER_INCH					10160
/* steps per degrees */
#define MCHN_STEPS_PER_DEG					4.444
/* number of ticks for acceleration/decceleration cycle */
#define MCHN_FEED_ACCEL_TICKS               1024
/* maximal feed rate (steps/second) */
#define MCHN_FEED_MAX                       (MCHN_STEPS_PER_MM * 30)


/* GCODE CONFIGURATION */
/* maximal number of digits in the integer part of the number */
#define GCODE_MAX_INT_DIGS					9
/* maximal number of digits in the fractional part of the number */
#define GCODE_MAX_FRAC_DIGS					9


#endif /* CNC_CONFIG_H_ */
