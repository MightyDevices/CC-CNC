/*
 * fixp.c
 *
 *  Created on: Oct 8, 2016
 *      Author: Tomek
 */

#include "fixp.h"

/* compute square root */
fixp_t fixp_sqrt(fixp_t x)
{
    /* temporary variables */
    uint32_t root = 0, rem = 0, div = 0, sq = (uint32_t)x;

    /* bit-by-bit sqare root computation */
    for (int i = 0; i < FIXP_INTBITS / 2 + FIXP_FRACBITS; i++) {
        /* next digit */
        root <<= 1;
        /* get two digits from input */
        rem = (rem << 2) | (sq >> 30); sq = sq << 2;
        /* test for appending '1' to current result */
        div = (root << 1) | 1;
        /* test passed? */
        if (div <= rem)
            rem -= div, root++;
    }

    /* return root */
    return root;
}

/* compute reciprocal */
fixp_t fixp_rec(fixp_t x)
{
    /* sign of the number */
	int sign = x > 0 ? 1 : -1;
	/* temporary variables */
	uint32_t abs_x = fixp_abs(x), rem = 1, res = 0;

    /* unable to invert zero */
    if (!x)
        return 0;

    /* bit by bit computation */
    for (int i = 0; i < FIXP_FRACBITS * 2; i++) {
        /* remainder is greater than divisor */
        if (rem >= abs_x)
            rem -= abs_x, res++;
        /* next digit */
        res <<= 1; rem <<= 1;
    }

    /* multiply by sign */
    return sign * res;
}
