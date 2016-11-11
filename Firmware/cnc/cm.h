/*
 * cm.h
 *
 *  Created on: Oct 7, 2016
 *      Author: Tomek
 */

#ifndef CNC_CM_H_
#define CNC_CM_H_

#include <util/fixp.h>
#include <cnc/words.h>

/* error codes */
/* no error */
#define CM_ERR_OK							0
/* unsupported feature */
#define CM_ERR_FAIL

/* feed modes */
#define CM_FEED_MODE_UNIT_PER_MIN			0
#define CM_FEED_MODE_INVERSE				1
/* spindle modes */
#define CM_SPINDLE_OFF						0
#define CM_SPINDLE_ON_CW					1
#define CM_SPINDLE_ON_CCW					2
/* cooling modes */
#define CM_COOLANT_OFF						0x01
#define CM_COOLANT_MIST					    0x02
#define CM_COOLANT_FLOOD					0x04
/* override mode */
#define CM_OVERRIDES_OFF					0
#define CM_OVERRIDES_ON						1
/* planes */
#define CM_PLANE_XY							0
#define CM_PLANE_XZ							1
#define CM_PLANE_YZ							2
/* planes */
#define CM_UNITS_INCH						0
#define CM_UNITS_MM							1
/* cutter radius compensation modes */
#define CM_CUTRCOMP_OFF						0
#define CM_CUTRCOMP_LEFT					1
#define CM_CUTRCOMP_RIGHT					2
/* tool offset modes */
#define CM_TOOLOFFS_OFF						0
#define CM_TOOLOFFS_ON						1
/* coordinate systems */
#define CM_COORDSYS_1						0
#define CM_COORDSYS_2						1
#define CM_COORDSYS_3						2
#define CM_COORDSYS_4						3
#define CM_COORDSYS_5						4
#define CM_COORDSYS_6						5
#define CM_COORDSYS_7						6
#define CM_COORDSYS_8						7
#define CM_COORDSYS_9						8
/* path control modes */
#define CM_PATHMODE_EX_PATH					0
#define CM_PATHMODE_EX_STOP					1
#define CM_PATHMODE_CONTINUOUS				2
/* distance modes */
#define CM_DIST_ABS							0
#define CM_DIST_INC							1
/* retract mode */
#define CM_RETRACT_INIT						0
#define CM_RETRACT_RPOINT					1
/* homing modes */
#define CM_HOME_1							0
#define CM_HOME_2							1
/* linear motion modes */
#define CM_LINMOT_FEED						0
#define CM_LINMOT_TRAVERSE					1
/* arc motion */
#define CM_ARCMOT_CW						0
#define CM_ARCMOT_CCW						1
/* program stop modes */
#define CM_PSTOP_STOP						0
#define CM_PSTOP_OPT_STOP					1
#define CM_PSTOP_SHUT_STOP					2
/* program end modes */
#define CM_PEND_END							0
#define CM_PEND_END_SHUT_RESET				1
/* coordinate system offsets mode */
#define CM_COORDOFFS_SET					0
#define CM_COORDOFFS_RESET_STORE			1
#define CM_COORDOFFS_RESET					2
#define CM_COORDOFFS_APPLY					3

/* state vector */
typedef struct {
	/* feed settings */
	int fmode, frate;
	/* currently selected plane */
	int plane;
	/* units, distance mode */
	int units, distance_mode;

	/* target position in absolute coordinates x, y, z, a, b, c */
	fixp_t pos[WORD_AXIS_NUM];

	/* currently selected coordinate system */
	int current_cs;
	/* coordinate system data */
	fixp_t csd[9][WORD_AXIS_NUM];
	/* offsets enabled? */
	int offsets_enabled;
	/* coordinate system offsets */
	fixp_t cso[WORD_AXIS_NUM];
} cm_t;

/* canonical machining */
extern cm_t cm;

/* selects feed mode */
int CM_SetFeedMode(int mode);
/* sets feed rate */
int CM_SetFeedRate(fixp_t rate);
/* adjusts spindle speed */
int CM_SetSpindleSpeed(fixp_t speed);
/* select tool */
int CM_SelectTool(fixp_t tool);
/* perform tool change */
int CM_ChangeTool(void);
/* enable disable spindle */
int CM_SetSpindleOnOff(int mode);
/* enable/disable cooling */
int CM_SetCoolingMode(int mode);
/* enable/disable overrides */
int CM_SetOverridesMode(int mode);
/* dwell */
int CM_Dwell(fixp_t period);
/* select active plane */
int CM_SetActivePlane(int mode);
/* select units */
int CM_SetUnits(int mode);
/* cutter radius compensation */
int CM_SetCutterRadiusCompensation(int mode, int flags, fixp_t tool);
/* tool length offset */
int CM_SetToolLengthOffset(int mode, fixp_t tool);
/* select coordinate system */
int CM_SelectCoordinateSystem(int system);
/* select path control mode */
int CM_SetPathControlMode(int mode);
/* select distance mode */
int CM_SetDistanceMode(int mode);
/* select retraction mode */
int CM_SetRetractMode(int mode);
/* perform a homing cycle */
int CM_Homing(int mode, uint32_t *flags, fixp_t *words);
/* change coordinate system data */
int CM_SetCoordinateSystemData(int system_num, uint32_t *flags, fixp_t *words);
/* set coordinate system offsets */
int CM_SetCoordinateSystemOffsets(int mode, uint32_t *flags, fixp_t *words);
/* performs linear motion */
int CM_LinearMotion(int absolute, int mode, uint32_t *flags, fixp_t *words);
/* perform arc motion */
int CM_ArcMotion(int mode, uint32_t *flags, fixp_t *words);
/* stop program execution */
int CM_ProgramStop(int mode);
/* end program execution */
int CM_ProgramEnd(int mode);

#endif /* CM_H_ */

