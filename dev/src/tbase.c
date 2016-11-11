/*
 * tbase.c
 *
 *  Created on: 01-01-2016
 *      Author: Tomek
 */

#include <stm32l1/rcc.h>
#include <stm32l1/timer.h>
#include <stm32l1/nvic.h>
#include <stm32l1/gpio.h>
#include <sys/err.h>
#include <sys/ev.h>
#include <sys/critical.h>
#include <cnc/mchn.h>

#define DEBUG
#include <dev/debug.h>

/* time base tick event */
ev_t tbase_ev;

/* interrupt service routine */
void TBase_Tim2Isr(void)
{
	/* clear interrupt */
	TIM2->SR = 0;

	/* notify other threads */
	Ev_Notify(&tbase_ev, 0);
}

/* initialize tests */
int TBase_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* enable timer 2 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* reset timer 2 */
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
	/* disable reset */
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

	/* configure timer 3 to generate time base */
	/* set default prescaler: 1MHz */
	TIM2->PSC = 31;
	/* default value: 50kHz */
	TIM2->ARR = 19;
	/* set configuration */
	TIM2->CR1 = TIM_CR1_URS | TIM_CR1_ARPE;
	/* update counters */
	TIM2->EGR = TIM_EGR_UG;
	/* enable the interrupt */
	TIM2->DIER = TIM_DIER_UIE;
	/* start timer */
	TIM2->CR1 |= TIM_CR1_CEN;

	/* set priority */
	NVIC_SETINTPRI(STM32_INT_TIM2, 0x00);
	/* enable interrupt */
	NVIC_ENABLEINT(STM32_INT_TIM2);

	/* exit critical section */
	Critical_Exit();

	/* report status */
	return EOK;
}

/* set prescaler */
int TBase_SetPrescaler(int value)
{
	/* apply prescaler value */
	TIM2->ARR = value;
	/* report status */
	return EOK;
}
