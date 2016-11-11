/*
 * fixp.h
 *
 *  Created on: 22-12-2012
 *      Author: Tomek
 */

#ifndef FIXP_H_
#define FIXP_H_

#include <stdint.h>

/* fixed point basic type */
typedef int32_t fixp_t;
/* fixed point accumulator */
typedef int64_t afixp_t;

/* fractional bits */
#define FIXP_FRACBITS               12
/* integer part bits */
#define FIXP_INTBITS				20
/* frac mask */
#define FIXP_FRACMASK	        	(0xFFF)
/* maximal value */
#define FIXP_MAXVAL					(0x7FFFFFFF)
/* minimal value */
#define FIXP_MINVAL					(0x80000000)
/* one */
#define FIXP_ONE					(0x1000)

/* fractional part */
#define fixp_f(x)					((x) & FIXP_FRACMASK)
/* integer part */
#define fixp_i(x)					((x) >> FIXP_FRACBITS)

/* add */
#define fixp_add(a, b)              ((a) + (b))
/* subtract */
#define fixp_sub(a, b)              ((a) - (b))
/* multiply */
#define fixp_mul(a, b)              ((fixp_t)(((afixp_t)(a) * (afixp_t)(b)) \
									>> FIXP_FRACBITS))
/* multiply with rounding */
#define fixp_mulr(a, b)              ((fixp_t)(((afixp_t)(a) * (afixp_t)(b) + (FIXP_ONE / 2)) \
									>> FIXP_FRACBITS))
/* divide */
#define fixp_div(a, b)              (((afixp_t)(a) << FIXP_FRACBITS) \
									/ (afixp_t)(b))
/* absolute value */
#define fixp_abs(A)                 ((A) < 0 ? -(A) : (A))

/* constant */
#define fixp(c)                     ((fixp_t)((c) * FIXP_ONE + \
									((c) >= 0 ? 0.5 : -0.5)))
/* float */
#define fixp_fl(x)					((float)(x) / (1 << FIXP_FRACBITS))

#endif /* FIXP_H_ */
