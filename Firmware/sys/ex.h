/*
 * ex.h
 *
 *  Created on: 03-07-2015
 *      Author: Tomek
 */

#ifndef SYS_EX_H_
#define SYS_EX_H_

#include <stdint.h>

/* load exclusively */
static inline __attribute__((always_inline)) uint32_t Ex_LDR(volatile void *ptr)
{
	/* result */
	uint32_t result;
	/* some assembly magic */
	__asm__ volatile (
		"ldrex		%0, [%1]		\n"
		: "=r" (result)
		: "r" (ptr)
	);
	/* report result */
	return result;
}

/* store exclusively */
static inline __attribute__((always_inline)) int Ex_STR(uint32_t value,
		volatile void *ptr)
{
	/* storage result */
	int result;
	/* some assembly magic */
	__asm__ volatile (
		"strex		%0, %1, [%2]	\n"
		: "=r" (result)
		: "r" (value), "r" (ptr)
	);
	/* report result */
	return -result;
}

#endif /* EX_H_ */
