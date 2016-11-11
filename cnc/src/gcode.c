/*
 * gcode.c
 *
 *  Created on: Oct 1, 2016
 *      Author: Tomek
 */

#include <cnc/gcode.h>
#include <cnc/words.h>
#include <cnc/cm.h>
#include <sys/err.h>
#include <cnc/config.h>
#include <util/string.h>

#define DEBUG
#include <dev/debug.h>

/* current motion mode */
static int motion;

/* returns true if c is a lower-case letter */
static int GCode_IsLowerCaseLetter(char c)
{
	return (c >= 'a' && c <= 'z');
}

/* returns true if c is a whitespace character */
static int GCode_IsWhitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
			c == '\f' || c == '\v');
}

/* returns true if c is a digit */
static int GCode_IsDigit(char c)
{
	return (c >= '0' && c <= '9');
}

/* perform line normalization: remove whitespace characters, convert all
 * letters to upper-case, convert numbers, get rid of the trailing/leading zeros
 * (0001 -> 1, 1. -> 1, .1 -> 0.1 ,1.0000 -> 1), remove plus sign, remove
 * comments, etc.. */
static int GCode_NormalizeLine(const char *in, char *out)
{
	/* source and destination pointers */
	const char *s; char *d;
	/* digit counter */
	int digs;

	/* remove all whitespace characters */
	for (s = in, d = out; *s; s++) {
		if (!GCode_IsWhitespace(*s))
			*d++ = *s;
	}
	/* terminate string */
	*d = 0;

	/* program start? disabled block? */
	if (out[0] == '/' || out[0] == '%') {
		/* reset output string, return size */
		out[0] = 0; return 1;
	}

	/* process every byte */
	for (s = out, d = out; *s; ) {
		/* convert all letters to uppercase */
		if (GCode_IsLowerCaseLetter(*s)) {
			*d++ = *s++ & 0xdf;
		/* eat up inline comments */
		} else if (*s == '(') {
			/* seek for the end of the comment */
			while (*s && *s != ')') s++;
			/* no closing bracket was detected */
			if (*s != ')')
				return ECOMMENT_BRACKET;
			/* eat up ending bracket */
			s++;
		/* eat up comments that go to the end of the line */
		} else if (*s == ';') {
			break;
		/* consume digits */
		} else if (GCode_IsDigit(*s) || *s == '.') {
			/* unsupported number format: there has to be a digit after '.'
			 * if it wasn't any before '.' */
			if (*s == '.' && !GCode_IsDigit(*(s + 1)))
				return ENUM_FORMAT;

			/* consume leading zeros */
			for (digs = 0; *s == '0'; ) s++, digs++;
			/* some meaningful digits to follow? */
			if (GCode_IsDigit(*s)) {
				/* copy these */
				for (digs = 0; GCode_IsDigit(*s) && digs < GCODE_MAX_INT_DIGS;)
					*d++ = *s++, digs++;
				/* number of digits exceeded? */
				if (digs == GCODE_MAX_INT_DIGS)
					return ENUM_FORMAT;
			/* only zeros were given, leave one zero */
			} else if (digs) {
				*d++ = '0';
			}

			/* got fractional part with following digit? */
			if (*s == '.' && GCode_IsDigit(*++s)) {
				/* copy the comma to the output */
				*d++ = '.';
				/* copy these */
				for (digs = 0; GCode_IsDigit(*s) && digs < GCODE_MAX_FRAC_DIGS;)
					*d++ = *s++, digs++;
				/* go in reverse and remove all trailing zeros */
				while (*(d - 1) == '0' && digs > 1) d--;
				/* remove comma if no digits were left */
				if (*(d - 1) == '.') d--;
			}

			/* another comma? wtf? */
			if (*s == '.')
				return ENUM_FORMAT;
		/* + and - signs must be followed by a number */
		} else if (*s == '+' || *s == '-') {
			/* copy the minus sign only */
			if (*s == '-') *d++ = *s;
			/* skip over */
			if (*++s != '.' && !GCode_IsDigit(*s))
				return ENUM_FORMAT;
		/* all other characters go here */
		} else {
			*d++ = *s++;
		}
	}

	/* add trailing zero */
	*d++ = '\0';
	/* return the length of the line */
	return (int)d - (int)out;
}

/* parse command word, returns the letter and a numerical value in Q32.32
 * format */
static int GCode_ParseWord(const char *in, char *letter, fixp_t *value)
{
	/* data pointer */
	const char *s = in;
	/* number sign, decimal point */
	int32_t sign = 1; uint32_t dec = 1;
	/* value */
	uint32_t v = 0, f = 0;

	/* no command letter given */
	if (*s == 0 || *s < 'A' || *s > 'Z')
		return ENO_LETTER;
	/* store letter */
	*letter = *s++;

	/* parse minus sign */
	if (*s == '-')
		sign = -1, s++;

	/* sign must be followed by digit or comma */
	if (!GCode_IsDigit(*s) && *s != '.')
		return ENUM_FORMAT;

	/* process the integer part */
	while (GCode_IsDigit(*s))
		v = v * 10 + *s - '0', s++;
	/* check value */
	if ((v >> 16) > fixp_i(FIXP_MAXVAL))
		return ENUM_FORMAT;
	/* store, clear v for further usage  */
	*value = v << FIXP_FRACBITS, v = 0;

	/* fractional part available? */
	if (*s == '.') {
		/* process as a normal integer number */
		while (GCode_IsDigit(*++s))
			v = v * 10 + *s - '0', dec *= 10;
		/* prepare binary representation of the fractional part */
		for (int i = 0; i < FIXP_FRACBITS; i++) {
			v = v * 2, f = f << 1;
			if (v >= dec)
				 f |= 1, v -= dec;
		}
		/* append fractional value */
		*value |= f;
	}

	/* make the number negative if minus sign was found */
	if (sign == -1)
		*value = -*value;

	/* return the length of the line */
	return (int)s - (int)in;
}

/* process a single g-word and update line structure accordingly */
static int GCode_ProcessGWord(fixp_t value, gcode_state_t *s)
{
	/* get the tenth parts from the value, these will get rounded there, but g
	 * words themselves require very limited precision... */
	int tenths = (fixp_f(value) * 10 + FIXP_ONE / 2) >> FIXP_FRACBITS;
	/* modal array index, modal flag to be written */
	uint32_t mi, mf;

	/* switch on integer part of command */
	switch (fixp_i(value)) {
	/* MODAL GROUP G0, non-modal commands */
	case 4 : mi = GCODE_MODAL_G0, mf = GCODE_G0_DWELL; break;
	case 10 : mi = GCODE_MODAL_G0, mf = GCODE_G0_ORIGIN; break;
	case 28 : mi = GCODE_MODAL_G0, mf = GCODE_G0_HOME_RET; break;
	case 30 : mi = GCODE_MODAL_G0, mf = GCODE_G0_HOME2_RET; break;
	case 53 : mi = GCODE_MODAL_G0, mf = GCODE_G0_MCHN_COORD; break;
	case 92 : {
		switch (tenths) {
		case 0 : mi = GCODE_MODAL_G0, mf = GCODE_G0_OFFSET_COORD; break;
		case 1 : mi = GCODE_MODAL_G0, mf = GCODE_G0_CANCEL_OFFS_RESET; break;
		case 2 : mi = GCODE_MODAL_G0, mf = GCODE_G0_CANCEL_OFFS; break;
		case 3 : mi = GCODE_MODAL_G0, mf = GCODE_G0_APPLY_OFFS; break;
		/* unknown g-code */
		default : return EUNS_GCODE; break;
		}
	} break;
	/* MODAL GROUP G1, motion commands */
	case 0 : mi = GCODE_MODAL_G1, mf = GCODE_G1_RAPID; break;
	case 1 : mi = GCODE_MODAL_G1, mf = GCODE_G1_LINEAR; break;
	case 2 : mi = GCODE_MODAL_G1, mf = GCODE_G1_ARC_CW; break;
	case 3 : mi = GCODE_MODAL_G1, mf = GCODE_G1_ARC_CCW; break;
	case 38 : {
		switch (tenths) {
		case 2 : mi = GCODE_MODAL_G1, mf = GCODE_G1_PROBE; break;
		/* unknown g-code */
		default : return EUNS_GCODE; break;
		}
	} break;
	case 80 : mi = GCODE_MODAL_G1, mf = GCODE_G1_CANCEL; break;
	case 81 : mi = GCODE_MODAL_G1, mf = GCODE_G1_DRILL; break;
	case 82 : mi = GCODE_MODAL_G1, mf = GCODE_G1_DRILL_DWELL; break;
	case 83 : mi = GCODE_MODAL_G1, mf = GCODE_G1_DRILL_PECK; break;
	case 84 : mi = GCODE_MODAL_G1, mf = GCODE_G1_RIGHT_TAP; break;
	case 85 : mi = GCODE_MODAL_G1, mf = GCODE_G1_BORING_NODWELL_FEED; break;
	case 86 : mi = GCODE_MODAL_G1, mf = GCODE_G1_BORING_SPINSTOP_RAPID; break;
	case 87 : mi = GCODE_MODAL_G1, mf = GCODE_G1_BORING_BACK; break;
	case 88 : mi = GCODE_MODAL_G1, mf = GCODE_G1_BORING_SPINSTOP_MAN; break;
	case 89 : mi = GCODE_MODAL_G1, mf = GCODE_G1_BORING_DWELL_FEED; break;
	/* MODAL GROUP G2, plane selection */
	case 17 : mi = GCODE_MODAL_G2, mf = GCODE_G2_XY; break;
	case 18 : mi = GCODE_MODAL_G2, mf = GCODE_G2_XZ; break;
	case 19 : mi = GCODE_MODAL_G2, mf = GCODE_G2_YZ; break;
	/* MODAL GROUP G3, distance mode */
	case 90 : mi = GCODE_MODAL_G3, mf = GCODE_G3_ABS; break;
	case 91 : mi = GCODE_MODAL_G3, mf = GCODE_G3_INC; break;
	/* MODAL GROUP G5, feed rate mode */
	case 93 : mi = GCODE_MODAL_G5, mf = GCODE_G5_INV_TIME; break;
	case 94 : mi = GCODE_MODAL_G5, mf = GCODE_G5_UNIT_PER_MIN; break;
	/* MODAL GROUP G6, units */
	case 20 : mi = GCODE_MODAL_G6, mf = GCODE_G6_INCH; break;
	case 21 : mi = GCODE_MODAL_G6, mf = GCODE_G6_MM; break;
	/* MODAL GROUP G7, cutter radius compensation */
	case 40 : mi = GCODE_MODAL_G7, mf = GCODE_G7_CANCEL; break;
	case 41 : mi = GCODE_MODAL_G7, mf = GCODE_G7_LEFT; break;
	case 42 : mi = GCODE_MODAL_G7, mf = GCODE_G7_RIGHT; break;
	/* MODAL GROUP G8, tool length offset */
	case 43 : mi = GCODE_MODAL_G8, mf = GCODE_G8_OFFSET; break;
	case 49 : mi = GCODE_MODAL_G8, mf = GCODE_G8_CANCEL; break;
	/* MODAL GROUP G10, return mode in canned cycles */
	case 98 : mi = GCODE_MODAL_G10, mf = GCODE_G10_INIT; break;
	case 99 : mi = GCODE_MODAL_G10, mf = GCODE_G10_RPOINT; break;
	/* MODAL GROUP G12, coordinate system selection */
	case 54 : mi = GCODE_MODAL_G12, mf = GCODE_G12_P1; break;
	case 55 : mi = GCODE_MODAL_G12, mf = GCODE_G12_P2; break;
	case 56 : mi = GCODE_MODAL_G12, mf = GCODE_G12_P3; break;
	case 57 : mi = GCODE_MODAL_G12, mf = GCODE_G12_P4; break;
	case 58 : mi = GCODE_MODAL_G12, mf = GCODE_G12_P5; break;
	case 59 : {
		switch (tenths) {
		case 0 : mi = GCODE_MODAL_G12, mf = GCODE_G12_P6; break;
		case 1 : mi = GCODE_MODAL_G12, mf = GCODE_G12_P7; break;
		case 2 : mi = GCODE_MODAL_G12, mf = GCODE_G12_P8; break;
		case 3 : mi = GCODE_MODAL_G12, mf = GCODE_G12_P9; break;
		/* unknown g-code */
		default : return EUNS_GCODE; break;
		}
	} break;
	/* MODAL GROUP G13, path control mode */
	case 61 : {
		switch (tenths) {
		case 0 : mi = GCODE_MODAL_G13, mf = GCODE_G13_EX_PATH; break;
		case 1 : mi = GCODE_MODAL_G13, mf = GCODE_G13_EX_STOP; break;
		/* unknown g-code */
		default : return EUNS_GCODE; break;
		}
	} break;
	case 64 : mi = GCODE_MODAL_G13, mf = GCODE_G13_CONTINUOUS; break;
	/* unknown g-code */
	default : return EUNS_GCODE; break;
	}

	/* in case of modal groups only one bit can be set per line. commands from
	 * single modal group are mutually exclusive */
	if (mi != GCODE_MODAL_G0 && s->modals[mi])
		return EMODAL_CONFLICT;

	/* Check for G10/28/30/92 being called with G0/1/2/3/38 on same block.
	 * this is not permitted by language spec. */
	if (s->modals[GCODE_MODAL_G0] & (GCODE_G0_ORIGIN | GCODE_G0_HOME_RET |
			GCODE_G0_HOME2_RET | GCODE_G0_OFFSET_COORD)) {
		/* got colliding motion commands? */
		if (s->modals[GCODE_MODAL_G1] & (GCODE_G1_RAPID | GCODE_G1_LINEAR |
				GCODE_G1_ARC_CW | GCODE_G1_ARC_CCW | GCODE_G1_PROBE)) {
			return EMODAL_CONFLICT;
		}
	}

	/* cannot perform homing to 1st and 2nd home position on the same time */
	if ((s->modals[GCODE_MODAL_G0] & GCODE_G0_HOME_RET) &&
		(s->modals[GCODE_MODAL_G0] & GCODE_G0_HOME2_RET)) {
		return EMODAL_CONFLICT;
	}

	/* set flag, apply value */
	s->modals[mi] = mf, s->mflags |= 1 << mi;

	/* report no error */
	return EOK;
}

/* process a single m-word and update state structure accordingly */
static int GCode_ProcessMWord(fixp_t value, gcode_state_t *s)
{
	/* modal array index, modal flag to be written */
	uint32_t mi, mf;

	/* switch on integer part of command */
	switch (fixp_i(value)) {
	/* MODAL GROUP 4, program stopping */
	case 0  : mi = GCODE_MODAL_M4, mf = GCODE_M4_STOP; break;
	case 1  : mi = GCODE_MODAL_M4, mf = GCODE_M4_OPT_STOP; break;
	case 2  : mi = GCODE_MODAL_M4, mf = GCODE_M4_PEND; break;
	case 30 : mi = GCODE_MODAL_M4, mf = GCODE_M4_PEND_SHUT_RESET; break;
	case 60 : mi = GCODE_MODAL_M4, mf = GCODE_M4_SHUT_STOP; break;
	/* MODAL GROUP 6, tool change */
	case 6  : mi = GCODE_MODAL_M6, mf = GCODE_M6_TOOLCHNG; break;
	/* MODAL GROUP 7, spindle */
	case 3  : mi = GCODE_MODAL_M7, mf = GCODE_M7_CW; break;
	case 4  : mi = GCODE_MODAL_M7, mf = GCODE_M7_CCW; break;
	case 5  : mi = GCODE_MODAL_M7, mf = GCODE_M7_STOP; break;
	/* MODAL GROUP 8, coolant */
	case 7  : mi = GCODE_MODAL_M8, mf = GCODE_M8_MIST; break;
	case 8  : mi = GCODE_MODAL_M8, mf = GCODE_M8_FLOOD; break;
	case 9  : mi = GCODE_MODAL_M8, mf = GCODE_M8_OFF; break;
	/* MODAL GROUP 9, override switches */
	case 48 : mi = GCODE_MODAL_M9, mf = GCODE_M9_ENABLE; break;
	case 49 : mi = GCODE_MODAL_M9, mf = GCODE_M9_DISABLE; break;
	/* unknown m-code */
	default : return EUNS_MCODE; break;
	}

	/* in case of modal groups only one bit can be set per line. commands from
	 * single modal group are mutually exclusive. M8 is an exception here  */
	if (mi != GCODE_MODAL_M8 && s->modals[mi])
		return EMODAL_CONFLICT;
	/* set flag, apply value */
	s->modals[mi] = mf, s->mflags |= 1 << mi;

	/* mist and flood cooling may be enable at the same time but a stop command
	 * shall not occur in the same line */
	if ((s->modals[GCODE_MODAL_M8] & GCODE_M8_OFF) &&
		(s->modals[GCODE_MODAL_M8] & (GCODE_M8_MIST | GCODE_M8_FLOOD))) {
		return EMODAL_CONFLICT;
	}

	/* report no error */
	return EOK;
}

/* process remaining parameter words */
static int GCode_ProcessParameterWord(char letter, fixp_t value,
		gcode_state_t *s)
{
	/* word index (in words array), flag */
	uint32_t wi;

	/* convert letter to word array index */
	switch (letter) {
	/* words (geometry related) */
	case 'X' : wi = WORD_X; break;
	case 'Y' : wi = WORD_Y; break;
	case 'Z' : wi = WORD_Z; break;
	case 'A' : wi = WORD_A; break;
	case 'B' : wi = WORD_B; break;
	case 'C' : wi = WORD_C; break;
	case 'I' : wi = WORD_I; break;
	case 'J' : wi = WORD_J; break;
	case 'K' : wi = WORD_K; break;
	case 'R' : wi = WORD_R; break;
	/* other words */
	case 'T' : wi = WORD_T; break;
	case 'F' : wi = WORD_F; break;
	case 'S' : wi = WORD_S; break;
	case 'P' : wi = WORD_P; break;
	case 'N' : wi = WORD_N; break;
	case 'L' : wi = WORD_L; break;
	case 'H' : wi = WORD_L; break;
	case 'D' : wi = WORD_D; break;
	case 'Q' : wi = WORD_Q; break;
	/* unknown letter code */
	default : return EUNS_LETTER_CODE; break;
	}

	/* it is unlikely to set the same parameter in a single line more than once,
	 * this might be a programmers mistake, let's inform about it */
	if (s->wflags & 1 << wi)
		return EWORD_DUPLICATE;
	/* 'other' words must be positive */
	if (wi >= WORD_T && value < 0)
		return EWORD_VALUE;
	/* line number has a maximal value */
	if (wi == WORD_N && fixp_i(value) > 100000)
		return EINVALID_LINE_NUM;

	/* set flag, update value */
	s->wflags |= 1 << wi; s->words[wi] = value;

	/* report status */
	return EOK;
}

/* process words that are present in line. they will get transformed to
 * gcode_state_t type here */
static int GCode_ProcessWords(const char *in, gcode_state_t *s)
{
	/* processing status */
	int rc;
	/* word letter, word numeric value */
	char letter; fixp_t value;

	/* process line */
	for (const char *p = in; *p; ) {
		/* try to parse word */
		rc = GCode_ParseWord(p, &letter, &value);
		/* error? */
		if (rc < 0)
			return rc;
		/* update pointer */
		p += rc;

		/* process word */
		switch (letter) {
		/* G - word */
		case 'G' : rc = GCode_ProcessGWord(value, s); break;
		/* M - word */
		case 'M' : rc = GCode_ProcessMWord(value, s); break;
		/* parameter words, X, Y, Z, and so on.. */
		default : rc = GCode_ProcessParameterWord(letter, value, s); break;
		}

		/* error? */
		if (rc < EOK)
			return rc;
	}

	/* report success */
	return EOK;
}

/* process a single line */
int GCode_ProcessLine(const char *in, gcode_state_t *s)
{
	/* input data pointer */
	const char *p = in;
	/* no line shall be longer than 256 bytes */
	char line[256];
	/* processing result code, line length*/
	int rc, len;

	/* get the length of the input string */
	for (p = in, len = 0; *p && len < 256; p++, len++);
	/* check length */
	if (len == 256)
		return ELINE_TOO_LONG;

	/* try to normalize line */
	rc = GCode_NormalizeLine(in, line);
	/* error while parsing? */
	if (rc < EOK)
		return rc;

	/* print values */
	dprintf("line: %s\n", line);

	/* process all words that are found in line */
	rc = GCode_ProcessWords(line, s);
	/* error while processing? */
	if (rc < EOK)
		return rc;

	/* process words */
	return EOK;
}

/* execute set feed rate step */
static int GCode_ExecuteSetFeedRateMode(gcode_state_t *s)
{
	/* assume mode */
	int mode = CM_FEED_MODE_UNIT_PER_MIN;

	/* no change requested? */
	if (!s->modals[GCODE_MODAL_G5])
		return EOK;

	/* inverse mode */
	if (s->modals[GCODE_MODAL_G5] == GCODE_G5_INV_TIME)
		mode = CM_FEED_MODE_INVERSE;

	/* apply selected mode */
	return CM_SetFeedMode(mode);
}

/* execute set feed rate step */
static int GCode_ExecuteSetFeedRate(gcode_state_t *s)
{
	/* no feed update? */
	if (!(s->wflags & WFLAG_F))
		return EOK;

	/* consume feed rate word */
	s->wflags &= ~WFLAG_F;
	/* execute */
	return CM_SetFeedRate(s->words[WORD_F]);
}

/* execute set spindle speed step */
static int GCode_ExecuteSetSpindleSpeed(gcode_state_t *s)
{
	/* no change requested */
	if (!(s->wflags & WFLAG_S))
		return EOK;

	/* consume spindle speed word */
	s->wflags &= ~WFLAG_S;
	/* execute */
	return CM_SetSpindleSpeed(s->words[WORD_S]);
}

/* execute tool selection */
static int GCode_ExecuteToolSelect(gcode_state_t *s)
{
	/* no change requested */
	if (!(s->wflags & WFLAG_T))
		return EOK;

	/* consume spindle tool word */
	s->wflags &= ~WFLAG_T;
	/* execute */
	return CM_SelectTool(s->words[WORD_T]);
}

/* execute tool change */
static int GCode_ExecuteToolChange(gcode_state_t *s)
{
	/* no change requested */
	if (!(s->modals[GCODE_MODAL_M6]))
		return EOK;

	/* perform change */
	return CM_ChangeTool();
}

/* execute spindle on/off request */
static int GCode_ExecuteSpindleOnOff(gcode_state_t *s)
{
	int mode = 0;

	/* no change requested */
	if (!(s->modals[GCODE_MODAL_M7]))
		return EOK;

	/* determine the mode of spindle operation */
	switch (s->modals[GCODE_MODAL_M7]) {
	case GCODE_M7_STOP : mode = CM_SPINDLE_OFF; break;
	case GCODE_M7_CW : mode = CM_SPINDLE_ON_CW; break;
	case GCODE_M7_CCW : mode = CM_SPINDLE_ON_CCW; break;
	}

	/* perform change */
	return CM_SetSpindleOnOff(mode);
}

/* execute coolant mode */
static int GCode_ExecuteCoolantMode(gcode_state_t *s)
{
	int mode = 0;

	/* no change requested */
	if (!s->modals[GCODE_MODAL_M8])
		return EOK;

	/* determine mode for canonical machining */
	if (s->modals[GCODE_MODAL_M8] & GCODE_M8_OFF) {
		mode |= CM_COOLANT_OFF;
	} else if (s->modals[GCODE_MODAL_M8] & GCODE_M8_MIST) {
		mode |= CM_COOLANT_MIST;
	} else if (s->modals[GCODE_MODAL_M8] & GCODE_M8_FLOOD) {
		mode |= CM_COOLANT_FLOOD;
	}

	/* coolant enable/disable */
	return CM_SetCoolingMode(mode);
}

/* execute overrides mode change */
static int GCode_ExecuteOverrideMode(gcode_state_t *s)
{
	int mode = CM_OVERRIDES_OFF;

	/* no change requested */
	if (!s->modals[GCODE_MODAL_M9])
		return EOK;

	/* on mode requested? */
	if (s->modals[GCODE_MODAL_M9] == GCODE_M9_ENABLE)
		mode = CM_OVERRIDES_ON;
	/* overrides enable/disable */
	return CM_SetOverridesMode(mode);
}

/* execute dwell */
static int GCode_ExecuteDwell(gcode_state_t *s)
{
	/* no dwell requested? */
	if (!(s->modals[GCODE_MODAL_G0] & GCODE_G0_DWELL))
		return EOK;

	/* need to have P word which specifies dwell duration in ms */
	if (!(s->wflags & WFLAG_P))
		return EWORD_MISSING;

	/* mark as consumed */
	s->wflags &= ~WFLAG_P;
	/* execute dwell */
	return CM_Dwell(s->words[WORD_P]);
}

/* execute active plane selection */
static int GCode_ExecutePlaneSelection(gcode_state_t *s)
{
	int mode = 0;

	/* no change of plane requested? */
	if (!s->modals[GCODE_MODAL_G2])
		return EOK;

	/* determine the mode of spindle operation */
	switch (s->modals[GCODE_MODAL_G2]) {
	case GCODE_G2_XY : mode = CM_PLANE_XY; break;
	case GCODE_G2_XZ : mode = CM_PLANE_XZ; break;
	case GCODE_G2_YZ : mode = CM_PLANE_YZ; break;
	}

	/* apply plane selection */
	return  CM_SetActivePlane(mode);
}

/* execute units change */
static int GCode_ExecuteUnitsChange(gcode_state_t *s)
{
	/* reset mode */
	int mode = CM_UNITS_INCH;

	if (!s->modals[GCODE_MODAL_G6])
		return EOK;

	/* millimeters requested? */
	if (s->modals[GCODE_MODAL_G6] == GCODE_G6_MM)
		mode = CM_UNITS_MM;

	/* implement units change */
	return CM_SetUnits(mode);
}

/* execute cutter radius compensation */
static int GCode_ExecuteCutterRadiusCompensation(gcode_state_t *s)
{
	int mode = 0, flags = 0;

	/* no change requested? */
	if (!s->modals[GCODE_MODAL_G7])
		return EOK;

	/* determine the mode of spindle operation */
	switch (s->modals[GCODE_MODAL_G7]) {
	case GCODE_G7_CANCEL : mode = CM_CUTRCOMP_OFF; break;
	case GCODE_G7_LEFT : mode = CM_CUTRCOMP_LEFT; break;
	case GCODE_G7_RIGHT : mode = CM_CUTRCOMP_RIGHT; break;
	}

	/* 'D' word was given? if so then use it */
	if (s->wflags & WFLAG_D)
		flags = WFLAG_D, s->wflags &= ~WFLAG_D;

	/* apply cutter radius compensation */
	return CM_SetCutterRadiusCompensation(mode, flags, s->words[WORD_D]);
}

/* execute tool length offset */
static int GCode_ExecuteToolLengthOffset(gcode_state_t *s)
{
	int mode = CM_TOOLOFFS_OFF;

	/* no change? */
	if (!s->modals[GCODE_MODAL_G8])
		return EOK;

	/* enable offset compensation? */
	if (s->modals[GCODE_MODAL_G8] == GCODE_G8_OFFSET) {
		/* h-word is required */
		if (!(s->wflags & WFLAG_H))
			return EWORD_MISSING;
		/* consume */
		s->wflags &= ~WFLAG_H;
		/* set mode */
		mode = CM_TOOLOFFS_ON;
	}

	/* apply tool length offset */
	return CM_SetToolLengthOffset(mode, s->words[WORD_H]);
}

/* execute coordinate system selection */
static int GCode_ExecuteCoordinateSystemSelection(gcode_state_t *s)
{
	int mode = 0;

	/* no change? */
	if (!s->modals[GCODE_MODAL_G12])
		return EOK;

	/* determine current coordinate system number */
	switch (s->modals[GCODE_MODAL_G12]) {
	case GCODE_G12_P1 : mode = CM_COORDSYS_1; break;
	case GCODE_G12_P2 : mode = CM_COORDSYS_2; break;
	case GCODE_G12_P3 : mode = CM_COORDSYS_3; break;
	case GCODE_G12_P4 : mode = CM_COORDSYS_4; break;
	case GCODE_G12_P5 : mode = CM_COORDSYS_5; break;
	case GCODE_G12_P6 : mode = CM_COORDSYS_6; break;
	case GCODE_G12_P7 : mode = CM_COORDSYS_7; break;
	case GCODE_G12_P8 : mode = CM_COORDSYS_8; break;
	case GCODE_G12_P9 : mode = CM_COORDSYS_9; break;
	}

	/* apply selection of new coordinate system */
	return CM_SelectCoordinateSystem(mode);
}

/* execute path control mode selection */
static int GCode_ExecutePathModeSelection(gcode_state_t *s)
{
	int mode = 0;

	/* no change? */
	if (!s->modals[GCODE_MODAL_G13])
		return EOK;

	/* determine mode */
	switch (s->modals[GCODE_MODAL_G13]) {
	case GCODE_G13_EX_PATH : mode = CM_PATHMODE_EX_PATH; break;
	case GCODE_G13_EX_STOP : mode = CM_PATHMODE_EX_STOP; break;
	case GCODE_G13_CONTINUOUS : mode = CM_PATHMODE_CONTINUOUS; break;
	}

	/* apply the selection of path control mode */
	return CM_SetPathControlMode(mode);
}

/* execute distance mode selection */
static int GCode_ExecuteDistanceModeSelection(gcode_state_t *s)
{
	int mode = CM_DIST_ABS;

	/* no change? */
	if (!s->modals[GCODE_MODAL_G3])
		return EOK;

	/* incremental mode */
	if (s->modals[GCODE_MODAL_G3] == GCODE_G3_INC)
		mode = CM_DIST_INC;

	/* apply distance mode selection */
	return CM_SetDistanceMode(mode);
}

/* execute retract mode selection */
static int GCode_ExecuteRetractModeSelection(gcode_state_t *s)
{
	int mode = CM_RETRACT_INIT;

	if (!s->modals[GCODE_MODAL_G10])
		return EOK;

	/* retract to r-point mode */
	if (s->modals[GCODE_MODAL_G10] == GCODE_G10_RPOINT)
		mode = CM_RETRACT_RPOINT;

	/* apply new setting */
	return CM_SetRetractMode(mode);
}

/* execute homing cycle */
static int GCode_ExecuteHomingCycle(gcode_state_t *s)
{
	/* assume mode */
	int mode = CM_HOME_1;

	/* no homing requested? */
	if (!(s->modals[GCODE_MODAL_G0] & (GCODE_G0_HOME_RET | GCODE_G0_HOME2_RET)))
		return EOK;

	/* 2nd home position requested? */
	if (s->modals[GCODE_MODAL_G0] == GCODE_G0_HOME2_RET)
		mode = CM_HOME_2;

	/* perform homing */
	return CM_Homing(mode, &s->wflags, s->words);
}

/* execute set coordinate system data */
static int GCode_ExecuteSetCoordinateSystemData(gcode_state_t *s)
{
	/* extract coordinate system number */
	int system_num = fixp_i(s->words[WORD_P]);

	/* no update requested? */
	if (!(s->modals[GCODE_MODAL_G0] & GCODE_G0_ORIGIN))
		return EOK;

	/* missing words */
	if (!(s->wflags & WFLAG_P) || !(s->wflags & WFLAG_L))
		return EWORD_MISSING;

	/* consume words */
	s->wflags &= ~(WFLAG_P | WFLAG_L);
	/* apply the modification of coordinate system data */
	return CM_SetCoordinateSystemData(system_num, &s->wflags, s->words);
}

/* execute axis offset settings update */
static int GCode_ExecuteSetCoordinateSystemOffset(gcode_state_t *s)
{
	/* mode of update */
	int mode = 0;

	/* what type of update is requested? */
	if (s->modals[GCODE_MODAL_G0] & GCODE_G0_OFFSET_COORD) {
		mode = CM_COORDOFFS_SET;
	} else if (s->modals[GCODE_MODAL_G0] & GCODE_G0_CANCEL_OFFS_RESET) {
		mode = CM_COORDOFFS_RESET_STORE;
	} else if (s->modals[GCODE_MODAL_G0] & GCODE_G0_CANCEL_OFFS) {
		mode = CM_COORDOFFS_RESET;
	} else if (s->modals[GCODE_MODAL_G0] & GCODE_G0_APPLY_OFFS) {
		mode = CM_COORDOFFS_APPLY;
	} else {
		return EOK;
	}

	/* apply the modification of coordinate system offset */
	return CM_SetCoordinateSystemOffsets(mode, &s->wflags, s->words);
}

/* execute motion */
static int GCode_ExecuteMotion(gcode_state_t *s)
{
	/* do we have motion command specified? if not, then use previous motion
	 * mode but only if there are some free axis words */
	if (!s->modals[GCODE_MODAL_G1]) {
		/* check if there are some axis words left */
		if (s->wflags & WFLAG_AXIS)
			s->modals[GCODE_MODAL_G1] = motion;
	} else {
		motion = s->modals[GCODE_MODAL_G1];
	}

	/* no error will be generated here */
	return EOK;
}

/* execute linear motion */
static int GCode_ExecuteLinearMotion(gcode_state_t *s)
{
	/* assume mode */
	int mode = CM_LINMOT_FEED, absolute = 0;

	/* no linear motion requested? */
	if (!(s->modals[GCODE_MODAL_G1] & (GCODE_G1_RAPID | GCODE_G1_LINEAR)))
		return EOK;

	/* traverse mode instead of feed? */
	if (s->modals[GCODE_MODAL_G1] == GCODE_G1_RAPID)
		mode = CM_LINMOT_TRAVERSE;

	/* motion in absolute coordinates */
	if (s->modals[GCODE_MODAL_G0] & GCODE_G0_MCHN_COORD)
		absolute = 1;

	/* perform linear motion */
	return CM_LinearMotion(absolute, mode, &s->wflags, s->words);
}

/* execute arc motion */
static int GCode_ExecuteArcMotion(gcode_state_t *s)
{
	/* switch on mode */
	int mode = CM_ARCMOT_CW;

	/* no arc motion */
	if (!(s->modals[GCODE_MODAL_G1] & (GCODE_G1_ARC_CW | GCODE_G1_ARC_CCW)))
		return EOK;

	/* counter-clockwise motion? */
	if (s->modals[GCODE_MODAL_G1] == GCODE_G1_ARC_CCW)
		mode = CM_ARCMOT_CCW;

	/* perform arc motion */
	return CM_ArcMotion(mode, &s->wflags, s->words);
}

/* execute program stop */
static int GCode_ExecuteProgramStop(gcode_state_t *s)
{
	/* stop mode */
	int mode = 0;

	/* no stop requested */
	if (!(s->modals[GCODE_MODAL_M4] & (GCODE_M4_STOP | GCODE_M4_OPT_STOP |
			GCODE_M4_SHUT_STOP)))
		return EOK;

	/* determine stop mode */
	switch (s->modals[GCODE_MODAL_M4]) {
	case GCODE_M4_STOP : mode = CM_PSTOP_STOP; break;
	case GCODE_M4_OPT_STOP : mode = CM_PSTOP_OPT_STOP; break;
	case GCODE_M4_SHUT_STOP : mode = CM_PSTOP_SHUT_STOP; break;
	}

	/* execute stop */
	return CM_ProgramStop(mode);
}

/* execute program end */
static int GCode_ExecuteProgramEnd(gcode_state_t *s)
{
	int mode = CM_PEND_END;
	/* no ending requested? */
	if (!(s->modals[GCODE_MODAL_M4] & (GCODE_M4_PEND |
			GCODE_M4_PEND_SHUT_RESET)))
		return EOK;

	if (s->modals[GCODE_MODAL_M4] == GCODE_M4_PEND_SHUT_RESET)
		mode = CM_PEND_END_SHUT_RESET;

	/* execute stop */
	return CM_ProgramEnd(mode);
}

/* checks if all commands have their parameters set correctly according to
 * current and next state vector (newly processed line), performs theoretical
 * execution in order given by NIST spec */
int GCode_ExecuteLine(gcode_state_t *s)
{
	/* 1. comments - dealt with during normalization */
	/* Nothing to do */

	/* 2. set feed rate mode */
	if (GCode_ExecuteSetFeedRateMode(s) < EOK)
		return EEXEC_FEED_MODE;

	/* 3. set feed rate */
	if (GCode_ExecuteSetFeedRate(s) < EOK)
		return EEXEC_FEED_RATE;

	/* 4. set spindle speed */
	if (GCode_ExecuteSetSpindleSpeed(s) < EOK)
		return EEXEC_SPINDLE_SPEED;

	/* 5. select tool */
	if (GCode_ExecuteToolSelect(s) < EOK)
		return EEXEC_TOOL;

	/* 6. change tool */
	if (GCode_ExecuteToolChange(s) < EOK)
		return EEXEC_TOOL;

	/* 7. spindle on/off - enable spindle relay, vfd or whatever */
	if (GCode_ExecuteSpindleOnOff(s) < EOK)
		return EEXEC_SPINDLE_ONOFF;

	/* 8. coolant on/off */
	if (GCode_ExecuteCoolantMode(s) < EOK)
		return EEXEC_COOLANT;

	/* 9. overrides enable/disable */
	if (GCode_ExecuteOverrideMode(s) < EOK)
		return EEXEC_OVERRIDES;

	/* 10. dwell - physical operation */
	if (GCode_ExecuteDwell(s) < EOK)
		return EEXEC_DWELL;

	/* 11. set active plane - logical operation */
	if (GCode_ExecutePlaneSelection(s) < EOK)
		return EEXEC_PLANE;

	/* 12. set length units - logical operation */
	if (GCode_ExecuteUnitsChange(s) < EOK)
		return EEXEC_UNITS;

	/* 13. cutter radius compensation */
	if (GCode_ExecuteCutterRadiusCompensation(s) < EOK)
		return EEXEC_CUT_RADIUS_COMP;

	/* 14. tool length offset */
	if (GCode_ExecuteToolLengthOffset(s) < EOK)
		return EEXEC_TOOL_LENGTH_OFFS;

	/* 15. select coordinate system - logical operation */
	if (GCode_ExecuteCoordinateSystemSelection(s) < EOK)
		return EEXEC_COORD_SELECTION;

	/* 16. select path control mode */
	if (GCode_ExecutePathModeSelection(s) < EOK)
		return EEXEC_PATH_CTRL;

	/* 17. select distance mode */
	if (GCode_ExecuteDistanceModeSelection(s) < EOK)
		return EEXEC_DISTANCE_MODE;

	/* 18. select retract mode */
	if (GCode_ExecuteRetractModeSelection(s) < EOK)
		return EEXEC_RETRACT;

	/* 19. remaining non modal actions */
	/* 19.1 homing */
	if (GCode_ExecuteHomingCycle(s) < EOK)
		return EEXEC_HOMING;
	/* 19.2 set coordinate system data */
	if (GCode_ExecuteSetCoordinateSystemData(s) < EOK)
		return EEXEC_COORD_DATA_CHANGE;
	/* 19.3 set axis offset */
	if (GCode_ExecuteSetCoordinateSystemOffset(s) < EOK)
		return EEXEC_AXIS_OFFSET;

	/* 20. perform motion */
	/* 20.1 prepare motion */
	if (GCode_ExecuteMotion(s) < EOK)
		return EEXEC_MOTION;
	/* 20.2 prepare linear motion */
	if (GCode_ExecuteLinearMotion(s) < EOK)
		return EEXEC_MOTION;
	/* 20.2 prepare arc motion */
	if (GCode_ExecuteArcMotion(s) < EOK)
		return EEXEC_MOTION;
	/* 20.3 cancel motion TODO */
	/* 20.4 probing TODO */
	/* 20.5 canned cycles TODO */

	/* 21 program stopping */
	/* 21.1 Stopping */
	if (GCode_ExecuteProgramStop(s) < EOK)
		return EEXEC_STOP;
	/* 21.2 Ending */
	if (GCode_ExecuteProgramEnd(s) < EOK)
		return EEXEC_STOP;

	/* check if we have any axis words left unprocessed, if so then we may have
	 * a problem */
	if (s->wflags)
		return EWORD_EXCESS;

	/* report status */
	return EOK;
}

/* main gcode input */
int GCode_Input(const char *str)
{
	int rc = EOK;
	/* gcode line */
	gcode_state_t s;
	/* reset */
	memset(&s, 0, sizeof(s));

	/* parse line */
	rc = GCode_ProcessLine(str, &s);
	/* unable to parse? */
	if (rc != EOK)
		return rc;

	/* execute line */
	rc = GCode_ExecuteLine(&s);
	/* unable to parse? */
	if (rc != EOK)
		return rc;

	/* report success */
	return EOK;
}
