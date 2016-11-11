/*
 * led.c
 *
 *  Created on: 28-02-2016
 *      Author: Tomek
 */

#include <dev/led.h>
#include <stm32l1/gpio.h>
#include <stm32l1/rcc.h>
#include <stm32l1/timer.h>
#include <stm32l1/nvic.h>
#include <sys/err.h>
#include <sys/critical.h>

#define DEBUG
#include <dev/debug.h>


/* busy led timer */
void Led_Tim10Isr(void)
{
	uint16_t sr = TIM10->SR;
	/* clear interrupts */
	TIM10->SR = ~sr;

	/* led on */
	if (sr & TIM_SR_UIF) {
		GPIOC->BSRR = GPIO_BSRR_BS_14;
	/* led off */
	} else {
		GPIOC->BSRR = GPIO_BSRR_BR_14;
	}
}

/* err led timer */
void Led_Tim11Isr(void)
{
	uint16_t sr = TIM11->SR;
	/* clear interrupts */
	TIM11->SR = ~sr;

	/* led on */
	if (sr & TIM_SR_UIF) {
		GPIOC->BSRR = GPIO_BSRR_BS_15;
	/* led off */
	} else {
		GPIOC->BSRR = GPIO_BSRR_BR_15;
	}
}

/* initialize leds */
int Led_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* enable gpioc */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	/* enable timer */
	RCC->APB2ENR |= RCC_APB2ENR_TIM11EN | RCC_APB2ENR_TIM10EN;

	/* reset moder */
	GPIOC->MODER &= ~(GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
	/* default pin state */
	GPIOC->BSRR = GPIO_BSRR_BR_14 | GPIO_BSRR_BR_15;
	/* apply moder */
	GPIOC->MODER |= GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;

	/* configure prescaler */
	TIM11->PSC = TIM10->PSC = 31999;
	/* disable interrupt generation for egr generated events */
	TIM11->CR1 = TIM10->CR1 = TIM_CR1_URS;
	/* update prescaler */
	TIM11->EGR = TIM10->EGR = TIM_EGR_UG;
	/* interrupts */
	TIM11->DIER = TIM10->DIER = TIM_DIER_UIE | TIM_DIER_CC1IE;

	/* interrupt routine */
	NVIC_SETINTPRI(STM32_INT_TIM10, 0x00);
	NVIC_SETINTPRI(STM32_INT_TIM11, 0x00);
	/* enable */
	NVIC_ENABLEINT(STM32_INT_TIM10);
	NVIC_ENABLEINT(STM32_INT_TIM11);

	/* exit critical section */
	Critical_Exit();

	/* report status */
	return EOK;
}

/* configure led blinking fro busy led */
void Led_BsyBlink(int on, int period)
{
	/* stop timer */
	TIM10->CR1 &= ~TIM_CR1_CEN;
	/* disable led */
	GPIOC->BSRR = GPIO_BSRR_BR_14;
	/* no period given? no led action */
	if (!period)
		return;

	/* reset timer */
	TIM10->CNT = 0;
	/* configure timing */
	TIM10->ARR = period; TIM10->CCR1 = on;
	/* enable led */
	GPIOC->BSRR = GPIO_BSRR_BS_14;
	/* start timer */
	TIM10->CR1 |= TIM_CR1_CEN;
}

/* configure led blinking for err led */
void Led_ErrBlink(int on, int period)
{
	/* stop timer */
	TIM11->CR1 &= ~TIM_CR1_CEN;
	/* disable led */
	GPIOC->BSRR = GPIO_BSRR_BR_15;
	/* no period given? no led action */
	if (!period)
		return;

	/* reset timer */
	TIM11->CNT = 0;
	/* configure timing */
	TIM11->ARR = period; TIM11->CCR1 = on;
	/* enable led */
	GPIOC->BSRR = GPIO_BSRR_BS_15;
	/* start timer */
	TIM11->CR1 |= TIM_CR1_CEN;
}

void Led_ErrToggle(void)
{
	if (GPIOC->IDR & GPIO_IDR_IDR_15) {
		GPIOC->BSRR = GPIO_BSRR_BR_15;
	} else {
		GPIOC->BSRR = GPIO_BSRR_BS_15;
	}
}
