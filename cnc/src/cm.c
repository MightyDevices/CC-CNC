/*
 * cm.c
 *
 *  Created on: Oct 7, 2016
 *      Author: Tomek
 */

#include <cnc/cm.h>
#include <cnc/mchn.h>
#include <cnc/spindle.h>
#include <cnc/config.h>
#include <util/fixp.h>
#include <sys/err.h>


#define DEBUG
#include <dev/debug.h>

/* canonical machining */
cm_t cm;


/* TODO
 * feed rate - DONE
 * inverse feed rate - DONE
 * circle radius format - DONE
 * acceleration/deceleration algorithms (jerk, s-curve) - DONE
 * naprawiæ ledy - DONE
 * obs³uga krañcówek
 * pwm - DONE
 * I2C1 receive - DONE
 * MCP23008 - DONE
 * naprawic osie - DONE
 * przyspieszanie w odniesieniu do max speed - DONE
 * inne min speed?  (uwzglêdniæ feed bo mo¿e byæ ma³y) - DONE
 * bug z osi¹ z w trakcie ruchu arc feed - DONE
 * */

/* convert position to steps */
static int32_t CM_PosToSteps(fixp_t x)
{
	/* conversion factor */
	fixp_t conv = fixp(MCHN_STEPS_PER_MM);
	/* convert from inches? */
	if (cm.units == CM_UNITS_INCH)
		conv = fixp(MCHN_STEPS_PER_INCH);

	/* perform fixed point multiplication with rounding and get rid of
	 * fractional part */
	int64_t steps = ((int64_t)x * conv + FIXP_ONE / 2) >> (FIXP_FRACBITS * 2);
	/* report number of steps */
	return steps;
}

/* convert rotation to steps */
static int32_t CM_RotToSteps(fixp_t x)
{
	/* conversion factor */
	fixp_t conv = fixp(MCHN_STEPS_PER_DEG);
	/* perform fixed point multiplication with rounding and get rid of
	 * fractional part */
	int64_t steps = ((int64_t)x * conv + FIXP_ONE / 2) >> (FIXP_FRACBITS * 2);
	/* report number of steps */
	return steps;
}

/* update target position */
static void CM_UpdateTargetPosition(int absolute, uint32_t flags, fixp_t *words)
{
	/* incremental mode? */
	if (cm.distance_mode == CM_DIST_INC) {
		/* add to current position */
		for (int i = WORD_X; i < WORD_C; i++)
			if (flags & 1 << i)
				cm.pos[i] += words[i];
	/* absolute mode? */
	} else {
		/* substitute */
		for (int i = WORD_X; i < WORD_C; i++)
			if (flags & 1 << i) {
				/* translate to absolute coordinates */
				cm.pos[i] = words[i];
				/* coordinate system is in use */
				if (!absolute) {
					cm.pos[i] += cm.csd[cm.current_cs][i];
					/* take offsets under account iff those are enabled */
					if (cm.offsets_enabled)
						cm.pos[i] += cm.cso[i];
				}
			}
	}

	/* display current target position */
	dprintf("target: %q, %q, %q - %q, %q, %q\n",
			cm.pos[WORD_X], cm.pos[WORD_Y],
			cm.pos[WORD_Z], cm.pos[WORD_A],
			cm.pos[WORD_B], cm.pos[WORD_C]);
}

/* selects feed mode */
int CM_SetFeedMode(int mode)
{
	/* display information */
	dprintf("feed mode: %d\n", mode);
	
	/* change of mode is requested? */
	if (cm.fmode != mode) {
        /* apply mode */
        cm.fmode = mode;
        /* reset feed rate */
        Mchn_SetFeedRate(0, 0);
	}

	/* report no error */
	return EOK;
}

/* sets feed rate */
int CM_SetFeedRate(fixp_t rate)
{
    /* argument, flags */
    uint32_t arg, fl = 0;
	/* display information */
    dprintf("feed rate: %q\n", rate);
	
	/* direct feed rate? */
	if (cm.fmode == CM_FEED_MODE_UNIT_PER_MIN) { 
        /* convert to steps per second */
        arg = CM_PosToSteps(rate) / 60;
        dprintf("feed sps = %d\n", arg);
    /* inverse feed mode? */
    } else {
        /* convert to number of ticks */
        arg = (uint64_t)rate * 60 * MCHN_TICK_FREQ >> FIXP_FRACBITS;
        /* set flag */
        fl = MCHN_FLAGS_IFRM;
        dprintf("feed ticks = %d\n", arg);
    }
    
	/* report no error */
	return Mchn_SetFeedRate(fl, arg);
}

/* adjusts spindle speed */
int CM_SetSpindleSpeed(fixp_t speed)
{
	/* display information */
	dprintf("spindle speed: %q\n", speed);
	/* report no error */
	return Spindle_SetRpms(speed);
}

/* select tool */
int CM_SelectTool(fixp_t tool)
{
	/* extract */
	int32_t t = fixp_i(tool);

	/* display information */
	dprintf("change tool: %d\n", t);
	/* report no error */
	return EOK;
}

/* perform tool change */
int CM_ChangeTool(void)
{
	/* display information */
	dprintf("change tool\n", 0);
	/* report no error */
	return EOK;
}

/* enable disable spindle */
int CM_SetSpindleOnOff(int mode)
{
	/* spindle mode, spindle direction */
	int state = SPINDLE_ON, dir = 0;
	/* display information */
	dprintf("spindle: %d\n", mode);

	/* clockwise spindle motion */
	if (mode == CM_SPINDLE_ON_CW) {
		dir = SPINDLE_CW;
	/* counter clockwise spindle motion */
	} else if (mode == CM_SPINDLE_ON_CCW) {
		dir = SPINDLE_CCW;
	/* spindle off */
	} else {
		state = SPINDLE_OFF;
	}

	/* apply */
	return Spindle_SetOnOff(state, dir);
}

/* enable/disable cooling */
int CM_SetCoolingMode(int mode)
{
	/* display information */
	dprintf("cooling: %d\n", mode);
	/* report no error */
	return EOK;
}

/* enable/disable overrides */
int CM_SetOverridesMode(int mode)
{
	/* display information */
	dprintf("overrides: %d\n", mode);
	/* report no error */
	return EOK;
}

/* dwell */
int CM_Dwell(fixp_t period)
{
	/* display information */
	dprintf("dwell: %q\n", period);
	/* convert to number of system ticks */
	uint32_t ticks = (period * MCHN_TICK_FREQ) >> FIXP_FRACBITS;

	/* report no error */
	return Mchn_Dwell(ticks, 0);
}

/* select active plane */
int CM_SetActivePlane(int mode)
{
	/* display information */
	dprintf("plane: %d\n", mode);

	/* store plane */
	cm.plane = mode;
	/* report no error */
	return EOK;
}

/* select units */
int CM_SetUnits(int units)
{
	/* conversion factor */
	fixp_t conv = fixp(25.4000);
	/* inches? */
	if (units == CM_UNITS_INCH)
		conv = fixp(0.03937);

	/* display information */
	dprintf("units: %d\n", units);

	/* no change requested */
	if (cm.units == units)
		return EOK;

	/* perform conversion for x-y-z */
	cm.pos[WORD_X] = fixp_mulr(cm.pos[WORD_X], conv);
	cm.pos[WORD_Y] = fixp_mulr(cm.pos[WORD_Y], conv);
	cm.pos[WORD_Z] = fixp_mulr(cm.pos[WORD_Z], conv);

	/* convert offsets */
	cm.cso[WORD_X] = fixp_mulr(cm.cso[WORD_X], conv);
	cm.cso[WORD_Y] = fixp_mulr(cm.cso[WORD_Y], conv);
	cm.cso[WORD_Z] = fixp_mulr(cm.cso[WORD_Z], conv);

	/* convert coordinate systems */
	for (int i = 0; i < 9; i++) {
		cm.csd[i][WORD_X] = fixp_mulr(cm.csd[i][WORD_X], conv);
		cm.csd[i][WORD_Y] = fixp_mulr(cm.csd[i][WORD_Y], conv);
		cm.csd[i][WORD_Z] = fixp_mulr(cm.csd[i][WORD_Z], conv);
	}

	/* store units */
	cm.units = units;

	/* report no error */
	return EOK;
}

/* cutter radius compensation */
int CM_SetCutterRadiusCompensation(int mode, int flags, fixp_t tool)
{
	/* convert */
	int32_t t = fixp_i(tool);
	/* display information */
	dprintf("cutter radius compensation: mode = %d, tool = %d\n", mode, t);
	/* report no error */
	return EOK;
}

/* tool length offset */
int CM_SetToolLengthOffset(int mode, fixp_t tool)
{
	/* convert */
	int32_t t = fixp_i(tool);
	/* display information */
	dprintf("tool length offset: mode = %d, tool = %d\n", mode, t);
	/* report no error */
	return EOK;
}

/* select coordinate system */
int CM_SelectCoordinateSystem(int system)
{
	/* display information */
	dprintf("coordinate system: %d\n", system);

	/* store current coordinate system info */
	cm.current_cs = system;
	/* report no error */
	return EOK;
}

/* select path control mode */
int CM_SetPathControlMode(int mode)
{
	/* display information */
	dprintf("path control mode: %d\n", mode);
	/* report no error */
	return EOK;
}

/* select distance mode */
int CM_SetDistanceMode(int mode)
{
	/* display information */
	dprintf("distance mode: %d\n", mode);

	/* store current distance mode */
	cm.distance_mode = mode;
	/* report no error */
	return EOK;
}

/* select retraction mode */
int CM_SetRetractMode(int mode)
{
	/* display information */
	dprintf("retract mode: %d\n", mode);
	/* report no error */
	return EOK;
}

/* perform a homing cycle */
int CM_Homing(int mode, uint32_t *flags, fixp_t *words)
{
//	/* at least one axis flag must be provided */
//	if ((*flags & WFLAG_AXIS) == 0)
//		return EWORD_MISSING;

	/* consume flags */
	*flags &= ~(WFLAG_AXIS);
	/* display information */
	dprintf("homing mode: %d, flags: %x\n", mode, *flags);
	dprintf("home: %q, %q, %q - %q, %q, %q\n",
			words[WORD_X], words[WORD_Y],
			words[WORD_Z], words[WORD_A],
			words[WORD_B], words[WORD_C]);

	/* report no error */
	return EOK;
}

/* change coordinate system data */
int CM_SetCoordinateSystemData(int system_num, uint32_t *flags, fixp_t *words)
{
	/* display information */
	dprintf("set coord system data mode: %d flags: %x\n", system_num, *flags);
	dprintf("coords: %q, %q, %q - %q, %q, %q\n",
			words[WORD_X], words[WORD_Y],
			words[WORD_Z], words[WORD_A],
			words[WORD_B], words[WORD_C]);

	/* TODO: support for different L values */
	if (system_num > 9 || system_num < 0)
		return EWORD_VALUE;

	/* store offsets */
	for (int i = 0; i < WORD_AXIS_NUM; i++)
		if (*flags & (1 << i))
			cm.csd[system_num][i] = words[i];

	/* consume flags */
	*flags &= ~(WFLAG_AXIS);

	/* report no error */
	return EOK;
}

/* set coordinate system offsets */
int CM_SetCoordinateSystemOffsets(int mode, uint32_t *flags, fixp_t *words)
{
	/* set offset */
	if (mode == CM_COORDOFFS_SET && !(*flags & WFLAG_AXIS))
		return EWORD_MISSING;

	/* display information */
	dprintf("change coord system offset mode: %d, flags = %x\n", mode, *flags);
	dprintf("offset: %q, %q, %q - %q, %q, %q\n",
			words[WORD_X], words[WORD_Y],
			words[WORD_Z], words[WORD_A],
			words[WORD_B], words[WORD_C]);

	/* switch on offset mode */
	switch (mode) {
	/* set offsets */
	case CM_COORDOFFS_SET : {
		/* pointer to current coordinate system */
		fixp_t *csd = cm.csd[cm.current_cs];
		/* store offsets */
		for (int i = 0; i < WORD_AXIS_NUM; i++)
			if (*flags & (1 << i))
				cm.cso[i] = (cm.pos[i] - csd[i]) - words[i];

		/* offsets are now enabled */
		cm.offsets_enabled = 1;
		/* consume flags */
		*flags &= ~(WFLAG_AXIS);
	} break;
	/* disable offsets and or clear memory */
	case CM_COORDOFFS_RESET_STORE : {
		/* disable offsets */
		cm.offsets_enabled = 0;
		/* zero out offset memory */
		cm.cso[WORD_X] = cm.cso[WORD_Y] = cm.cso[WORD_Z]= 0;
		cm.cso[WORD_A] = cm.cso[WORD_B] = cm.cso[WORD_C]= 0;
	} break;
	/* disable offsets */
	case CM_COORDOFFS_RESET : {
		cm.offsets_enabled = 0;
	} break;
	/* restore offsets */
	case CM_COORDOFFS_APPLY : {
		cm.offsets_enabled = 1;
	} break;
	}

	/* report no error */
	return EOK;
}

/* performs linear motion */
int CM_LinearMotion(int absolute, int mode, uint32_t *flags, fixp_t *words)
{
	/* destination array */
	int32_t dst[MCHN_AXES_NUM];
	/* flags */
	uint32_t fl = MCHN_FLAGS_TRAVERSE;

	/* it is an error when all axis words are omitted */
	if (!(*flags & WFLAG_AXIS))
		return EWORD_MISSING;

	/* display information */
	dprintf("linear motion: abs: %d, mode: %d, flags %x\n", absolute, mode,
			*flags);
	dprintf("linear: %q, %q, %q - %q, %q, %q\n",
			words[WORD_X], words[WORD_Y],
			words[WORD_Z], words[WORD_A],
			words[WORD_B], words[WORD_C]);

	/* update target position */
	CM_UpdateTargetPosition(absolute, *flags, words);

	/* convert position */
	dst[MCHN_AXIS_X] = CM_PosToSteps(cm.pos[WORD_X]);
	dst[MCHN_AXIS_Y] = CM_PosToSteps(cm.pos[WORD_Y]);
	dst[MCHN_AXIS_Z] = CM_PosToSteps(cm.pos[WORD_Z]);
	/* convert rotation */
	dst[MCHN_AXIS_A] = CM_RotToSteps(cm.pos[WORD_A]);
	dst[MCHN_AXIS_B] = CM_RotToSteps(cm.pos[WORD_B]);
	dst[MCHN_AXIS_C] = CM_RotToSteps(cm.pos[WORD_C]);

	/* consume flags */
	*flags &= ~(WFLAG_AXIS);

	/* normal feed */
	if (mode == CM_LINMOT_FEED)
		fl = 0;

	/* start linear feed */
	return Mchn_LinearFeed(dst, fl, 0);
}

/* perform arc motion */
int CM_ArcMotion(int mode, uint32_t *flags, fixp_t *words)
{
	/* required axes words */
	uint32_t req = 0;
	/* destination */
	int32_t dst[MCHN_AXES_NUM], ox = 0, oy = 0, fl = MCHN_FLAGS_CCW;

	/* required words depend on currently selected plane */
	switch (cm.plane) {
	/* x-y plane selected */
	case CM_PLANE_XY : {
		req = WFLAG_X | WFLAG_Y | WFLAG_I | WFLAG_J;
		ox = *flags & WFLAG_I ? words[WORD_I] : words[WORD_X];
		oy = *flags & WFLAG_J ? words[WORD_J] : words[WORD_Y];
	} break;
	/* x-z plane selected */
	case CM_PLANE_XZ : {
		req = WFLAG_X | WFLAG_Z | WFLAG_I | WFLAG_K;
		ox = *flags & WFLAG_I ? words[WORD_I] : words[WORD_X];
		oy = *flags & WFLAG_K ? words[WORD_K] : words[WORD_Z];
	} break;
	/* y-z plane selected */
	case CM_PLANE_YZ : {
		req = WFLAG_Y | WFLAG_Z | WFLAG_J | WFLAG_K;
		ox = *flags & WFLAG_J ? words[WORD_J] : words[WORD_Y];
		oy = *flags & WFLAG_K ? words[WORD_K] : words[WORD_Z];
	} break;
	}

	/* both modes selected? */
	if ((*flags & WFLAG_R) && (*flags & WFLAG_IJK))
		return EWORD_EXCESS;

	/* check for x-y-z words */
	if ((*flags & req & WFLAG_XYZ) == 0)
		return EWORD_MISSING;

	/* center arc mode */
	if ((*flags & WFLAG_R) == 0) {
		/* check for i-j-k words */
		if ((*flags & req & WFLAG_IJK) == 0)
			return EWORD_MISSING;
	/* radius mode - need to convert to center arc format */
	} else {
        /* radius */
        fixp_t r = words[WORD_R];
        /* octant code, center on the left as seen from current point towards 
         * the end point? */
        int oc = 0, left = !((r > 0) ^ (mode == CM_ARCMOT_CCW));
        /* since no 'ijk' words are present then we have the endpoint 
         * coordinates in ox and oy variables */
        fixp_t ex = ox, ey = oy;
        /* midpoint in relation to starting point 'a*' and end point 'b*' */
        fixp_t ax, ay, bx, by, t;
        /* initial distance values (from starting- and end point), radius, 
         * all squared */
        afixp_t ds, de, r2 = (afixp_t)r * r;
        
        /* get the absolute part of r */
        if (r < 0) r = -r;
        
        /* bring the endpoint to the first octant */
        if (ey <  0) ey = -ey, oc |= 4, left = !left;
        if (ex <  0) ex = -ex, oc |= 2, left = !left;
        if (ey > ex) t = ey, ey = ex, ex = t, oc |= 1, left = !left;
        
        /* copmpute midpoints */
        ax = ex / 2, ay = ey / 2;
        bx = ax - ex,  by = ay - ey;
        /* prepare initial distance values */
        ds = (afixp_t)ax * ax + (afixp_t)ay * ay;
        de = (afixp_t)bx * bx + (afixp_t)by * by;
        
        /* search for solution */
        for (; r && ds != r2; r >>= 1) {
            /* step up/step down? (y direction) */
            t = ds < r2 ? r : -r;
            /* update error values */
            ds += (afixp_t)2 * ay * t + (afixp_t)t * t;
            de += (afixp_t)2 * by * t + (afixp_t)t * t;
            /* update coordinates */
            ay += t, by += t;
            
            /* X axis motion */
            for (fixp_t _r = r; _r && ds != de; _r = _r >> 1) {
                /* step left/step right? (x direction) */
                t = ds < de ? _r : -_r;
                /* update error values */
                ds += (afixp_t)2 * ax * t + (afixp_t)t * t;
                de += (afixp_t)2 * bx * t + (afixp_t)t * t;
                /* update coordinates */
                ax += t, bx += t;
            }
        }
        
        /* flip the point if needed */
        if (!left)
            ax = ex - ax, ay = ey - ay;
        /* bring back to original octant */
        if (oc & 1) t = ay, ay = ax, ax = t;
        if (oc & 2) ax = -ax;
        if (oc & 4) ay = -ay;
        
        /* store result */
        ox = ax, oy = ay;
	}

	/* display information */
	dprintf("arc motion: mode: %d, flags %x\n", mode, *flags);
	dprintf("arc: %q, %q, %q - %q, %q, %q\n",
			words[WORD_X], words[WORD_Y],
			words[WORD_Z], words[WORD_A],
			words[WORD_B], words[WORD_C]);
	dprintf("arc: %q, %q, %q - %q, %q, %q\n",
			words[WORD_I], words[WORD_J],
			words[WORD_K], words[WORD_R]);
    dprintf("arc: ox %q, oy %q\n", ox, oy);

	/* update target position */
	CM_UpdateTargetPosition(0, *flags, words);

	/* convert position */
	dst[MCHN_AXIS_X] = CM_PosToSteps(cm.pos[WORD_X]);
	dst[MCHN_AXIS_Y] = CM_PosToSteps(cm.pos[WORD_Y]);
	dst[MCHN_AXIS_Z] = CM_PosToSteps(cm.pos[WORD_Z]);
	/* convert rotation */
	dst[MCHN_AXIS_A] = CM_RotToSteps(cm.pos[WORD_A]);
	dst[MCHN_AXIS_B] = CM_RotToSteps(cm.pos[WORD_B]);
	dst[MCHN_AXIS_C] = CM_RotToSteps(cm.pos[WORD_C]);

	/* convert offsets */
	ox = CM_PosToSteps(ox);
	oy = CM_PosToSteps(oy);

	/* clockwise rotation */
	if (mode == CM_ARCMOT_CW)
		fl = 0;

	/* consume all axis words */
	*flags &= ~(WFLAG_AXIS | req | WFLAG_R);

	/* perform arc feed */
	if (Mchn_ArcFeed(dst, req & WFLAG_XYZ, ox, oy, fl, 0))
		return EFATAL;
	/* for situations that arc is slightly off  (due to rounding) */
	if (Mchn_LinearFeed(dst, 0, 0))
		return EFATAL;

	/* done processing */
	return EOK;
}

/* stop program execution */
int CM_ProgramStop(int mode)
{
	/* display information */
	dprintf("program stop mode: %d\n", mode);
	/* report no error */
	return EOK;
}

/* end program execution */
int CM_ProgramEnd(int mode)
{
	/* display information */
	dprintf("program end mode: %d\n", mode);
	/* report no error */
	return EOK;
}
