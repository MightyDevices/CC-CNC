/*
 * critical.h
 *
 *  Created on: 26-08-2015
 *      Author: Tomek
 */

#ifndef SYS_CRITICAL_H_
#define SYS_CRITICAL_H_


/* enter critical section */
static inline __attribute__((always_inline)) void Critical_Enter(void)
{
	__asm__ volatile (
		"MSR 	basepri, %0\n"
		:
		: "r" (16)
	);
}

/* exit critical section */
static inline __attribute__((always_inline)) void Critical_Exit(void)
{
	__asm__ volatile (
		"MSR 	basepri, %0\n"
		:
		: "r" (0)
	);
}


#endif /* SYS_CRITICAL_H_ */
