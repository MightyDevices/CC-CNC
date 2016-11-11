/*
 * extimux.c
 *
 *  Created on: 31-08-2015
 *      Author: Tomek
 */

#include <stm32l1/rcc.h>
#include <stm32l1/exti.h>
#include <stm32l1/nvic.h>
#include <stm32l1/syscfg.h>
#include <sys/critical.h>
#include <sys/err.h>

/* routines */
#include <dev/mcp23008.h>

/* exti interrupt mux for lines 5 to 9 */
void ExtiMux_Exti5_9Isr(void)
{
	/* get pending register */
//	uint32_t pr = EXTI->PR;

//	/* bluenrg interrupt */
//	if (pr & EXTI_PR_PR6)
//		BlueNRG_Exti6Isr();
//
//	/* lsm9ds0 interrupt */
//	if (pr & EXTI_PR_PR7)
//		LSM9DS0_Exti7Isr();
}

/* exti interrupt mux for lines 10 to 15 */
void ExtiMux_Exti10_15Isr(void)
{
	/* get pending register */
	uint32_t pr = EXTI->PR;

	/* bluenrg interrupt */
	if (pr & EXTI_PR_PR13)
		MCP23008_Exti13Isr();
}

/* initialize exti mux */
int ExtiMux_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* enable syscfg */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* set priority */
	NVIC_SETINTPRI(STM32_INT_EXTI5_9, 0xf0);
	/* enable */
	NVIC_ENABLEINT(STM32_INT_EXTI5_9);

	/* set priority */
	NVIC_SETINTPRI(STM32_INT_EXTI10_15, 0xf0);
	/* enable interrupt */
	NVIC_ENABLEINT(STM32_INT_EXTI10_15);

	/* exit critical section */
	Critical_Exit();

	/* report status */
	return EOK;
}

