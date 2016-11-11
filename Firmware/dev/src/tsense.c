/*
 * tsense.c
 *
 *  Created on: Oct 23, 2016
 *      Author: Tomek
 */

#include <sys/err.h>
#include <stm32l1/gpio.h>
#include <stm32l1/rcc.h>
#include <sys/critical.h>

/* tool sensor init */
int TSense_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* enable gpio */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	/* configure as output */
	GPIOC->MODER &= ~GPIO_MODER_MODER13;

	/* exit critical section */
	Critical_Exit();

	/* report status */
	return EOK;
}

/* get state of tool sensor */
int TSense_GetState(void)
{
	/* report status */
	return GPIOC->IDR & GPIO_IDR_IDR_13 ? 0 : 1;
}
