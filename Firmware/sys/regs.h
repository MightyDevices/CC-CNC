/*
 * regs.h
 *
 *  Created on: 26-08-2015
 *      Author: Tomek
 */

#ifndef SYS_REGS_H_
#define SYS_REGS_H_

/* read stack pointer */
static inline __attribute__((always_inline)) uint32_t Regs_ReadMSP(void)
{
	/* result */
	uint32_t result;
	/* some assembly magic */
	__asm__ volatile (
		"mrs		%0, msp			\n"
		: "=r" (result)
		:
	);

	/* report result */
	return result;
}


/* read Interrupt Control State Register */
static inline __attribute__((always_inline)) uint32_t Regs_ReadIPSR(void)
{
	/* result */
	uint32_t result;
	/* some assembly magic */
	__asm__ volatile (
		"mrs		%0, ipsr		\n"
		: "=r" (result)
		:
	);

	/* report result */
	return result;
}

#endif /* SYS_REGS_H_ */
