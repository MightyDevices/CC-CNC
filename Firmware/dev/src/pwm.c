/*
 * pwm.c
 *
 *  Created on: Oct 22, 2016
 *      Author: Tomek
 */

#include <stm32l1/rcc.h>
#include <stm32l1/timer.h>
#include <stm32l1/nvic.h>
#include <stm32l1/gpio.h>
#include <sys/err.h>
#include <sys/critical.h>

/* initialize pwm */
int Pwm_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* enable gpiob */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	/* enable timer */
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	/* reset moder */
	GPIOB->MODER &= ~(GPIO_MODER_MODER6);
	/* configure as timer driven */
	GPIOB->AFRL &= ~(0xf << 6*4);
	GPIOB->AFRL |=  (0x2 << 6*4);
	/* apply moder */
	GPIOB->MODER |= GPIO_MODER_MODER6_1;

	/* no prescaling */
	TIM4->PSC = 0;
	/* 1ms period = 1kHz */
	TIM4->ARR = 31999;
	/* initial pwm value = 0% duty cycle */
	TIM4->CCR1 = 0;
	/* mode: pwm1, preload enable */
	TIM4->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
	/* enable output */
	TIM4->CCER = TIM_CCER_CC1E;
	/* perform update */
	TIM4->EGR = TIM_EGR_UG;
	/* enable timer */
	TIM4->CR1 = TIM_CR1_CEN;

	/* exit critical section */
	Critical_Exit();
	/* report status */
	return EOK;
}

/* sets duty cycle */
int Pwm_SetDutyCycle(int dc)
{
	/* argument out of range */
	if (dc > 100 || dc < 0)
		return EFATAL;

	/* compute value */
	int32_t ccr = (TIM4->ARR + 1) * dc / 100;
	/* apply */
	TIM4->CCR1 = ccr;

	/* all ok */
	return EOK;
}
