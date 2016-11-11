/*
 * mchn.c
 *
 *  Created on: Oct 7, 2016
 *      Author: Tomek
 */

#include <dev/tbase.h>
#include <util/fixp.h>
#include <util/abs.h>
#include <util/minmax.h>
#include <cnc/mchn.h>
#include <cnc/config.h>
#include <cnc/motors.h>
#include <sys/err.h>
#include <stdint.h>

#define DEBUG
#include <dev/debug.h>

/* idle */
#define STATE_IDLE							0
/* linear feed */
#define STATE_LINEAR						1
/* arc motion is performed */
#define STATE_ARC							2
/* dwell */
#define STATE_DWELL							3

/* linear motion block */
static struct {
	/* dominant axis, general direction */
	int32_t dom, dir;
	/* Bresenham's line algorithm: number of steps, error */
	uint32_t steps[MCHN_AXES_NUM];
	/* axis error */
	int64_t err[MCHN_AXES_NUM];
} lin;

/* arc motion block */
static struct {
	/* arc plane */
	int ax, ay;
	/* center offset */
	int32_t ox, oy;
	/* error, feed accumulators */
	int64_t err, feed;
} arc;

/* feed rate control */
static struct {
    /* feed rate accumulators */
    fixp_t x, y, z;
    /* target feed rate, correction factor */
    fixp_t trgt, cf;
} fr;

/* current state */
static int state;
/* number of steps */
static uint32_t steps, steps_left;
/* direction and step bit mask */
static uint32_t next_dir, next_step;
/* current position */
static int32_t pos[MCHN_AXES_NUM] = {0, 0, 0, 0, 0, 0};
/* current feed rate */
static fixp_t feed;
/* flags */
static uint32_t flags;
/* callback */
static cb_t callback;

/* 64 bit version of integer square root algorithm */
static uint32_t Mchn_Sqrt64(uint64_t x)
{
	/* temporary variables */
	uint32_t root = 0;
	uint64_t div, rem = 0;

	/* process 32 pairs of digits */
	for (int i = 0; i < 32; i++) {
		/* next digit */
		root <<= 1;
		/* get two digits from input */
		rem = (rem << 2) | (x >> 62); x = x << 2;
		/* test for appending '1' to current result */
		div = (root << 1) | 1;
		/* test passed? */
		if (div <= rem)
			rem -= div, root++;
	}

	/* return root */
	return root;
}

/* perform single linear feed step */
static void Mchn_StepLin(uint32_t *step, uint32_t *dir)
{
	/* direction is constant over whole linear approximation */
	*dir = lin.dir;

	/* process all non dominant axes */
	for (int i = 0; i < MCHN_AXES_NUM; i++) {
		/* update error */
		lin.err[i] += (int64_t)2 * lin.steps[i] * feed;
		/* advance by one step? */
		if (lin.err[i] >= 0)
			*step |= 1 << i, lin.err[i] -= (int64_t)2 * steps * FIXP_ONE;
	}

	/* one step processed */
	if (*step & 1 << lin.dom)
		steps_left--;
}

/* arc stepping */
static void Mchn_StepArc(uint32_t *step, uint32_t *dir)
{
    /* octant look up table */
    int oc_lut[] = {0, 1, 3, 2, 7, 6, 4, 5}, oc = 0;
    /* current coordinates relative to origin */
    int32_t x = pos[arc.ax] - arc.ox, y = pos[arc.ay] - arc.oy, t;
    /* counter clockwise motion flag */
    int ccw = (flags & MCHN_FLAGS_CCW) != 0;
    
    /* convert to 1st octant */
    if (y < 0) y = -y, oc |= 4;
    if (x < 0) x = -x, oc |= 2;
    if (y > x) t = y, y = x, x = t, oc |= 1;
    
    /* update feed counter */
    arc.feed += feed;
    /* feed counter exceeded */
    if (arc.feed >= FIXP_ONE) {
        /* get the step direction */
        int step = (ccw ^ oc_lut[oc]) & 1 ? 1 : -1;
        /* move the dominant axis */
        arc.err += (2 * y * step) + 1, y += step;
        /* auxilary axis needs to be moved? */
        if (abs(arc.err) >= x)
            arc.err += (-2 * x *step) + 1, x -= step;
        /* consume a step */
        steps_left--, arc.feed -= FIXP_ONE;
    }

    /* restore coordinates */
    if (oc & 1) t = y, y = x, x = t;
    if (oc & 2) x = -x;
    if (oc & 4) y = -y;
    
    /* get absolute position */
    x += arc.ox, y += arc.oy;
    
    /* set step bitmask */
    if (pos[arc.ax] != x) *step |= 1 << arc.ax; 
    if (pos[arc.ay] != y) *step |= 1 << arc.ay;
    /* set dir bitmask */
    if (pos[arc.ax] <= x) *dir  |= 1 << arc.ax;
    if (pos[arc.ay] <= y) *dir  |= 1 << arc.ay;
    
    /* apply direction mask */
    *dir |= lin.dir;
    /* now for the linear motion */
	for (int i = 0; i < MCHN_AXES_NUM; i++) {
		/* leave circular motion axes alone */
		if (i == arc.ax || i == arc.ay)
			continue;
		/* update error */
		lin.err[i] += (int64_t)2 * lin.steps[i] * feed;
		/* advance by one step? */
		if (lin.err[i] >= 0)
			*step |= 1 << i, lin.err[i] -= (int64_t)2 * steps * FIXP_ONE;
	}
}

/* dwelling step */
static void Mchn_StepDwell(uint32_t *step, uint32_t* dir)
{
	/* no stepping during dwell */
	*step = *dir = 0;
	/* one step processed */
	steps_left--;
}

/* update current position based on step and direction bitmap */
static void Mchn_UpdatePosition(uint32_t *step, uint32_t *dir)
{
	/* process all axes */
	for (int i = 0; i < MCHN_AXES_NUM; i++) {
		/* step bit set? */
		if (*step & 1 << i)
			pos[i] += *dir & (1 << i) ? 1 : -1;
	}

	/* drive motors */
	Motors_Step(*step, *dir);
	/* reset variables */
	*step = *dir = 0;
}

/* update feed rate */
static void Mchn_UpdateFeed(uint32_t step)
{
    /* max feed rate */
    fixp_t max_fr = MCHN_FEED_MAX * FIXP_ONE / MCHN_TICK_FREQ;
    
    /* evaluate factor that compensates for multi-axis motion: f.e. if 
     * x and y axes are both being moved (diagonal motion) then we would 
     * have an effective feed rate of sqrt(2)*feed which may result in a 
     * catastrophy :(. This piece of code alayses the use of xyz axes end 
     * estimates the correction factor by estimating euclidean norm: 
     * norm = sqrt(x^2+y^2+z^2) */
    if (step & 1 << MCHN_AXIS_X) fr.x++;
    if (step & 1 << MCHN_AXIS_Y) fr.y++;
    if (step & 1 << MCHN_AXIS_Z) fr.z++;
    /* got at least 16 ticks collected? */
    if (fr.x == 16 || fr.y == 16 || fr.z == 16) {
        /* norm sqared */
        fixp_t sq = fr.x * fr.x + fr.y * fr.y + fr.z * fr.z;
        /* to avoid computing sqrt we use this linear interpolation, 
         * results are 1/sqrt(1) -> 1, 1/sqrt(2) -> 0.75, 1/sqrt(3) -> 0.5 */
        fr.cf = ((FIXP_ONE / 2) * (sq - 256)) >> 9;
        /* reset counters */
        fr.x = fr.y = fr.z = 0;
    }
    
    /* acceleration/deceleration profile */
    /* get current step number */
    uint32_t cstep = steps - steps_left;

    /* acceleration: less than a half of motion performed? */
    if (cstep < steps / 2 && cstep < MCHN_FEED_ACCEL_TICKS) {
        feed = max_fr * cstep / MCHN_FEED_ACCEL_TICKS;
    /* decceleration */
    } else if (cstep >= steps / 2 && steps_left < MCHN_FEED_ACCEL_TICKS) {
        feed = max_fr * steps_left / MCHN_FEED_ACCEL_TICKS;
    /* target feed rate phase */
    } else {
        /* apply target feed rate */
        feed = max_fr;
    }

    /* limit feed rate if not traversing */
    if ((flags & MCHN_FLAGS_TRAVERSE) == 0 && feed > fr.trgt)
        feed = fr.trgt;

    /* use the correction factor */
    feed = fixp_mul(feed, (FIXP_ONE - fr.cf));
    /* sanity limit so that machine does not stall */
    feed = max(feed, min(250, (flags & MCHN_FLAGS_TRAVERSE ? max_fr : fr.trgt)));
}

/* perform a step */
static void Mchn_Step(void *arg)
{
	/* no processing in idle mode */
	if (state == STATE_IDLE)
		return;

	/* update current position based on step and direction bitmap. this is
	 * done just after calling isr, before heavy conditional code, so we have
	 * no jitter */
	Mchn_UpdatePosition(&next_step, &next_dir);
	/* display position */
//	dprintf_i("%s: position = %d, %d, %d, %d, %d\n", name[state], pos[0],
//			pos[1], pos[2], steps, steps_left);

	/* last motion completed? */
	if (steps_left == 0) {
		/* inverse feed rate mode is specific for this motion only */
		if (flags & MCHN_FLAGS_IFRM)
            fr.trgt = 0;
        /* reset flags && state */
        state = STATE_IDLE; flags = 0;

		/* sync call */
		if (callback == CB_NULL) {
			callback = CB_NONE;
		/* async call was made? */
		} else if (callback != CB_NONE) {
			callback(0);
		}
    }

	/* linear stepping */
	if (state == STATE_LINEAR) {
		Mchn_StepLin(&next_step, &next_dir);
	/* arc stepping */
	} else if (state == STATE_ARC) {
		Mchn_StepArc(&next_step, &next_dir);
	/* dwelling */
	} else if (state == STATE_DWELL) {
		Mchn_StepDwell(&next_step, &next_dir);
	}

	/* reset drive signal */
	Motors_StepReset();
	/* update current feed rate according to motion direction */
	Mchn_UpdateFeed(next_step);
}

/* set feed rate in steps per second */
int Mchn_SetFeedRate(uint32_t fl, int32_t feed_rate)
{    
    /* normal feed rate mode */
    if ((fl & MCHN_FLAGS_IFRM) == 0) {
        /* upper boundary */
        if (feed_rate > MCHN_FEED_MAX)
            feed_rate = MCHN_FEED_MAX;
        /* convert to fixp */
        feed_rate = feed_rate * FIXP_ONE / MCHN_TICK_FREQ;
    /* inverse feed rate mode */
    } else {
        /* store flag */
        flags |= MCHN_FLAGS_IFRM;
    }
    
    /* store information */
    fr.trgt = feed_rate;
    dprintf("fr.trgt = %d\n", fr.trgt);
    /* report status */
    return EOK;
}

/* dwell */
int Mchn_Dwell(uint32_t ticks, cb_t cb)
{
	/* async operation? */
	int32_t sync = cb == CB_NULL;

	/* store callback */
	callback = cb;
	/* store overall number of steps */
	steps = steps_left = ticks;

	/* this will start the state machine */
	state = STATE_DWELL;

	/* sync call? stall here till operation is finished */
	while (sync && callback == CB_NULL);

    /* report status */
    return EOK;
}

/* schedule linear motion */
int Mchn_LinearFeed(int32_t dst[MCHN_AXES_NUM], uint32_t fl, cb_t cb)
{
	/* index of dominant axis, maximal number of counts, async operation? */
	int sync = cb == CB_NULL;
	/* maximal number of steps */
	uint32_t steps_max = 0;

	/* if no feed rate is given then a motion would took forever... */
	if (!(fl & MCHN_FLAGS_TRAVERSE) && !fr.trgt)
		return EFATAL;

	/* store callback */
	callback = cb;
	/* reset global direction */
	lin.dir = 0;
	/* store probe */
	flags |= fl;

	/* prepare line algorithm data structure */
	for (int i = 0; i < MCHN_AXES_NUM; i++) {
		/* store direction bit */
		if (dst[i] > pos[i])
			lin.dir |= 1 << i;
		/* store step count */
		lin.steps[i] = abs(dst[i] - pos[i]);
		/* found new candidate for dominant axis? */
		if (lin.steps[i] > steps_max)
			steps_max = lin.steps[i], lin.dom = i;
	}

	/* store overall number of steps */
	steps = steps_left = steps_max;
	/* initialize error value */
	for (int i = 0; i < MCHN_AXES_NUM; i++) {
		lin.err[i] = -((int64_t)steps * FIXP_ONE);
    }

    /* convert inverse feed rate to normal feed rate */
    if (flags & MCHN_FLAGS_IFRM) {
        /* convert to direct feedrate */
        fr.trgt = (uint64_t)steps * FIXP_ONE / fr.trgt;
        /* avoid stalling the machine */
        if (fr.trgt == 0) 
            fr.trgt++;
    }

	dprintf("steps = %d, %d %d %d\n", steps, dst[0], dst[1], dst[2]);

	/* this will start the state machine */
	state = STATE_LINEAR;

    /* stepping loop */
	while (sync && callback == CB_NULL);

	/* report status */
	return EOK;
}

/* prepare for arc feed (which may include linear feed as well) */
int Mchn_ArcFeed(int32_t dst[MCHN_AXES_NUM], int axis, int32_t ox, int32_t oy,
    uint32_t fl, cb_t cb)
{
	/* async operation? */
	int sync = cb == CB_NULL;
	
    /* extract x axis identifier */
	int ax = axis & (1 << MCHN_AXIS_X) ? MCHN_AXIS_X : MCHN_AXIS_Y;
	/* extract y axis identifier */
	int ay = axis & (1 << MCHN_AXIS_Z) ? MCHN_AXIS_Z : MCHN_AXIS_Y;
	/* current position relative to circle center */
	int32_t x = -ox, y = -oy;
	/* end position relative to circle center */
	int32_t ex = dst[ax] - (pos[ax] + ox);
	int32_t ey = dst[ay] - (pos[ay] + oy);
	/* clockwise motion */
	int ccw = (fl & MCHN_FLAGS_CCW) != 0;
	
    /* if no feed rate is given then a motion would took forever... */
	if (!(fl & MCHN_FLAGS_TRAVERSE) && !fr.trgt)
		return EFATAL;
	
	/* store callback */
	callback = cb;
	/* store flags */
    flags |= fl;
	
	/* store arc plane information */
	arc.ax = ax, arc.ay = ay;
	/* store absolute coordinates of arc center */
	arc.ox = (pos[ax] + ox), arc.oy = (pos[ay] + oy);
	/* reset feed accumulator */
	arc.feed = 0;
	/* reset error counter, store flags */
	arc.err = 0;
	
	/* reset linear feed direction */
	lin.dir = 0;
	
	/* reset overall steps number */
	steps = 0;
	
	/* radius squared */
	int64_t e, r2 = (int64_t)x*x + (int64_t)y*y;
	/* number of steps per octant (where x or y is dominant axis, dominant = 
	 * each step always involves stepping in dominant axis direction) */
	int32_t steps_x, steps_y, n;
	/* n = sqrt(r2/2) - assumed number of steps in octant */
    steps_x = steps_y = n = Mchn_Sqrt64(r2 >> 1);
    /* let's check how much n*n*2 differs from actual r2, this shall be
     * always positive since our n comes from integer version of sqrt (which
     * does 'floor' (round down) the result) */
    e = r2 - (int64_t)2 * n * n;
    /* (n-1)^2 = n^2 - 2n + 1, which means that every time we take a step
     * forward (away from the circle center) in x or y axis we decrease the
     * error by (2n-1). our goal here is to get the error as close to zero as
     * possible then it is rational to take a step back when e > (2n-1) / 2
     * that is when  e > n. the order of correction (here steps_x before
     * steps_y) comes from the fact that we account steps that go over the
     * diagonals to octants where y is the dominant axis */
    if (e > n) steps_x++, e -= 2*n - 1;
    if (e > n) steps_y++, e -= 2*n - 1;
    
    /* compute number of steps from starting point, to the next octant */
	int _sx = x, _sy = y, _ex = ex, _ey = ey, soc = 0, eoc = 0;
    /* octant look up table */
    int oc_lut[] = {0, 1, 3, 2, 7, 6, 4, 5};

    /* bring the starting point back to the 1st octant, and determine original
     * octant number (encoded with gray code) */
    if (_sy < 0)   _sy = -_sy, soc |= 4;
    if (_sx < 0)   _sx = -_sx, soc |= 2;
    if (_sy > _sx) _sy =  _sx, soc |= 1;
    /* decode gray code */
    soc = oc_lut[soc];
    /* calculate number of steps till next octant will be reached */
    steps += (soc ^ ccw) & 1 ? steps_y - _sy : _sy;

    /* bring the end point back to the 1st octant, and determine original
     * octant number (encoded with gray code) */
    if (_ey < 0)   _ey = -_ey, eoc |= 4;
    if (_ex < 0)   _ex = -_ex, eoc |= 2;
    if (_ey > _ex) _ey =  _ex, eoc |= 1;
    /* decode gray code */
    eoc = oc_lut[eoc];
    /* calculate number of steps till next octant will be reached */
    steps += (eoc ^ ccw) & 1 ? _ey : steps_y - _ey;
    
    /* motion within a single octant */
    if (soc == eoc && ((_ey >= _sy) ^ !ccw ^ (eoc & 1))) {
    	steps = steps - steps_y;
    /* multiple octants motion */
    } else {
        /* sum all other octants */
        while (1) {
            /* next octant */
            soc = (soc + (ccw ? 1 : -1)) & 7;
            /* ending octant reached? */
            if (soc == eoc)
                break;
            /* magic trick with gray code */
            steps += oc_lut[soc] & 1 ? steps_x : steps_y;
        }
    }
    
    /* got no steps? draw full circle instead */
    if (!steps)
        steps = 4 * (steps_x + steps_y); 
	
    /* prepare linear motion: this will update all axes, but stepping routine
	 * will only deal with those which are not involved in circular motion */
	for (int i = 0; i < MCHN_AXES_NUM; i++) {
		/* leave circular motion axes alone */
		if (i == ax || i == ay)
			continue;

		/* store direction bit */
		if (dst[i] > pos[i])
			lin.dir |= 1 << i;
		/* store step count */
		lin.steps[i] = abs(dst[i] - pos[i]);
		/* sanity limit exceeded */
		if (lin.steps[i] > steps)
            return EFATAL;
		/* initial value */
		lin.err[i] = -(int64_t)steps * FIXP_ONE;
	}
	
    /* convert inverse feed rate to normal feed rate */
    if (flags & MCHN_FLAGS_IFRM) {
        /* convert to direct feedrate */
        fr.trgt = (uint64_t)steps * FIXP_ONE / fr.trgt;
        /* avoid stalling the machine */
        if (fr.trgt == 0) 
            fr.trgt++;
    }
	
	/* update steps left variable */
    steps_left = steps;
	/* this will start the stepping module */
	state = STATE_ARC;
	
	dprintf("steps = %d lin.dir = %x\n", steps, lin.dir);
	
    /* stepping loop */
	while (sync && callback == CB_NULL);
	
	/* report status */
	return EOK;
}

/* initialize machining function */
int Mchn_Init(void)
{
	/* register for events */
	Ev_RegisterCallback(&tbase_ev, Mchn_Step);

	/* report status */
	return EOK;
}
