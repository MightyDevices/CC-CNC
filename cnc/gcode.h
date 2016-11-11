/*
 * gcode.h
 *
 *  Created on: Oct 1, 2016
 *      Author: Tomek
 */

#ifndef GCODE_H_
#define GCODE_H_

#include <util/fixp.h>
#include <cnc/words.h>

/* modal group flags */
#define GCODE_MFLAG_MG0						0x00000001
#define GCODE_MFLAG_MG1						0x00000002
#define GCODE_MFLAG_MG2						0x00000004
#define GCODE_MFLAG_MG3						0x00000008
#define GCODE_MFLAG_MG4						0x00000010
#define GCODE_MFLAG_MG5						0x00000020
#define GCODE_MFLAG_MG6						0x00000040
#define GCODE_MFLAG_MG7						0x00000080
#define GCODE_MFLAG_MG8						0x00000100
#define GCODE_MFLAG_MG10					0x00000200
#define GCODE_MFLAG_MG11					0x00000400
#define GCODE_MFLAG_MG12					0x00000800
#define GCODE_MFLAG_MG13					0x00001000

/* modal group array indices */
#define GCODE_MODAL_G0						0
#define GCODE_MODAL_G1						1
#define GCODE_MODAL_G2						2
#define GCODE_MODAL_G3						3
#define GCODE_MODAL_G5						4
#define GCODE_MODAL_G6						5
#define GCODE_MODAL_G7						6
#define GCODE_MODAL_G8						7
#define GCODE_MODAL_G10						8
#define GCODE_MODAL_G12						9
#define GCODE_MODAL_G13						10
#define GCODE_MODAL_M4						11
#define GCODE_MODAL_M6						12
#define GCODE_MODAL_M7						13
#define GCODE_MODAL_M8						14
#define GCODE_MODAL_M9						15

/* MODAL GROUP G0, non-modal commands */
#define GCODE_G0_DWELL						0x00000001
#define GCODE_G0_ORIGIN						0x00000002
#define GCODE_G0_HOME_RET					0x00000004
#define GCODE_G0_HOME2_RET					0x00000008
#define GCODE_G0_MCHN_COORD					0x00000010
#define GCODE_G0_OFFSET_COORD				0x00000020
#define GCODE_G0_CANCEL_OFFS_RESET			0x00000040
#define GCODE_G0_CANCEL_OFFS				0x00000080
#define GCODE_G0_APPLY_OFFS					0x00000100

/* MODAL GROUP G1, motion commands */
#define GCODE_G1_RAPID						0x00000001
#define GCODE_G1_LINEAR						0x00000002
#define GCODE_G1_ARC_CW						0x00000004
#define GCODE_G1_ARC_CCW					0x00000008
#define GCODE_G1_CANCEL						0x00000010
#define GCODE_G1_PROBE						0x00000020
#define GCODE_G1_DRILL						0x00000040
#define GCODE_G1_DRILL_DWELL				0x00000080
#define GCODE_G1_DRILL_PECK					0x00000100
#define GCODE_G1_RIGHT_TAP					0x00000200
#define GCODE_G1_BORING_NODWELL_FEED		0x00000400
#define GCODE_G1_BORING_SPINSTOP_RAPID		0x00000800
#define GCODE_G1_BORING_BACK				0x00001000
#define GCODE_G1_BORING_SPINSTOP_MAN		0x00002000
#define GCODE_G1_BORING_DWELL_FEED			0x00004000

/* MODAL GROUP G2, plane selection */
#define GCODE_G2_XY							0x00000001
#define GCODE_G2_XZ							0x00000002
#define GCODE_G2_YZ							0x00000004

/* MODAL GROUP G3, distance mode */
#define GCODE_G3_ABS						0x00000001
#define GCODE_G3_INC						0x00000002

/* MODAL GROUP G5, feed rate mode */
#define GCODE_G5_INV_TIME					0x00000001
#define GCODE_G5_UNIT_PER_MIN				0x00000002

/* MODAL GROUP G6, units */
#define GCODE_G6_INCH						0x00000001
#define GCODE_G6_MM							0x00000002

/* MODAL GROUP G7, cutter radius compensation */
#define GCODE_G7_CANCEL						0x00000001
#define GCODE_G7_LEFT						0x00000002
#define GCODE_G7_RIGHT						0x00000004

/* MODAL GROUP G8, tool length offset */
#define GCODE_G8_OFFSET						0x00000001
#define GCODE_G8_CANCEL						0x00000002

/* MODAL GROUP G10, return mode in canned cycles */
#define GCODE_G10_INIT						0x00000001
#define GCODE_G10_RPOINT					0x00000002

/* MODAL GROUP G12, coordinate system selection */
#define GCODE_G12_P1						0x00000001
#define GCODE_G12_P2						0x00000002
#define GCODE_G12_P3						0x00000004
#define GCODE_G12_P4						0x00000008
#define GCODE_G12_P5						0x00000010
#define GCODE_G12_P6						0x00000020
#define GCODE_G12_P7						0x00000040
#define GCODE_G12_P8						0x00000080
#define GCODE_G12_P9						0x00000100

/* MODAL GROUP G13, path control mode */
#define GCODE_G13_EX_PATH					0x00000001
#define GCODE_G13_EX_STOP					0x00000002
#define GCODE_G13_CONTINUOUS				0x00000004

/* MODAL GROUP M4, program stopping */
#define GCODE_M4_STOP						0x00000001
#define GCODE_M4_OPT_STOP					0x00000002
#define GCODE_M4_PEND						0x00000004
#define GCODE_M4_PEND_SHUT_RESET			0x00000008
#define GCODE_M4_SHUT_STOP					0x00000010

/* MODAL GROUP M6, tool change */
#define GCODE_M6_TOOLCHNG					0x00000001

/* MODAL GROUP M7, spindle */
#define GCODE_M7_CW							0x00000001
#define GCODE_M7_CCW						0x00000002
#define GCODE_M7_STOP						0x00000004

/* MODAL GROUP M8, coolant */
#define GCODE_M8_MIST						0x00000001
#define GCODE_M8_FLOOD						0x00000002
#define GCODE_M8_OFF						0x00000004

/* MODAL GROUP M9, override switches */
#define GCODE_M9_ENABLE						0x00000001
#define GCODE_M9_DISABLE					0x00000002

/* state vector */
typedef struct {
	/* flags that indicate presence of data in modal and word arrays */
	uint32_t mflags, wflags;
	/* commands divided into modal groups */
	uint32_t modals[16];
	/* words */
	fixp_t words[WORD_NUM];
} gcode_state_t;

/* process a single line */
int GCode_ProcessLine(const char *in, gcode_state_t *s);
/* checks if all commands have their parameters set correctly according to
 * current and next state vector (newly processed line), performs theoretical
 * execution in order given by NIST spec */
int GCode_ExecuteLine(gcode_state_t *s);
/* main gcode input */
int GCode_Input(const char *str);

#endif /* GCODE_H_ */

