/*
 * atomic.h
 *
 *  Created on: 04-07-2015
 *      Author: Tomek
 */

#ifndef SYS_ATOMIC_H_
#define SYS_ATOMIC_H_

#include <sys/ex.h>
#include <sys/critical.h>

/* increment register */
static inline __attribute__((always_inline)) uint32_t Atomic_INC(
		volatile void *ptr)
{
	/* previous value */
	uint32_t prev_val;
	volatile uint32_t *p = ptr;

	/* ensure atomicity */
	Critical_Enter();
	/* exchange values */
	prev_val = *p, *p = prev_val + 1;
	/* exit critical section */
	Critical_Exit();

	/* return value before incrementing */
	return prev_val;
}

/* increment register */
static inline __attribute__((always_inline)) uint32_t Atomic_DEC(void *ptr)
{
	/* previous value */
	uint32_t prev_val;
	volatile uint32_t *p = ptr;

	/* ensure atomicity */
	Critical_Enter();
	/* exchange values */
	prev_val = *p, *p = prev_val - 1;
	/* exit critical section */
	Critical_Exit();

	/* return value before decrementing */
	return prev_val;
}

/* and register */
static inline __attribute__((always_inline)) uint32_t Atomic_AND(uint32_t value,
		volatile void *ptr)
{
	/* previous value */
	uint32_t prev_val;
	volatile uint32_t *p = ptr;

	/* ensure atomicity */
	Critical_Enter();
	/* exchange values */
	prev_val = *p, *p = prev_val & value;
	/* exit critical section */
	Critical_Exit();

	/* return value before anding */
	return prev_val;
}

/* or register */
static inline __attribute__((always_inline)) uint32_t Atomic_OR(uint32_t value,
		volatile void *ptr)
{
	/* previous value */
	uint32_t prev_val;
	volatile uint32_t *p = ptr;

	/* ensure atomicity */
	Critical_Enter();
	/* exchange values */
	prev_val = *p, *p = prev_val | value;
	/* exit critical section */
	Critical_Exit();

	/* return value before oring */
	return prev_val;
}

#endif /* ATOMIC_H_ */
