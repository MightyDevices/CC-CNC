/*
 * systime.c
 *
 *  Created on: 11-06-2013
 *      Author: Tomek
 */

#include <dev/watchdog.h>
#include <sys/err.h>
#include <sys/time.h>
#include <stm32l1/rcc.h>
#include <stm32l1/timer.h>
#include <stm32l1/stm32l1.h>

/* reset time base */
int SysTime_Init(void)
{
	/* enable tim5 clock */
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

	/* set prescaler */
	TIM5->PSC = 31999;
	/* set autoreload value */
	TIM5->ARR = 0xffffffff;
	/* reset value */
	TIM5->CNT = 0;
	/* set update event */
	TIM5->EGR = TIM_EGR_UG;
	/* enable timer */
	TIM5->CR1 = TIM_CR1_CEN;

	/* not much could go wrong here */
	return EOK;
}

/* deinitialize system tick timer */
int SysTime_Deinit(void)
{
	/* disable timer */
	TIM5->CR1 &= ~TIM_CR1_CEN;
	/* disable tim5 clock */
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM5EN;

	/* not much to do here */
	return EOK;
}

/* delay in ms */
void SysTime_Delay(uint32_t ms)
{
	/* get current time */
	uint32_t x = TIM5->CNT;
	/* blocking wait */
	while (TIM5->CNT - x < ms)
		Watchdog_Kick();
}

/* set current time */
void SysTime_SetTime(uint32_t t)
{
	/* apply new setting */
	TIM5->CNT = t;
}
