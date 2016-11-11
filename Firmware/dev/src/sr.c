/*
 * sr.c
 *
 *  Created on: Aug 28, 2016
 *      Author: Tomek
 */

#include <stm32l1/rcc.h>
#include <stm32l1/gpio.h>
#include <sys/err.h>
#include <sys/critical.h>
#include <dev/sr.h>
#include <dev/spi1.h>

/* shift register initialize */
int SR_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* enable porta */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* reset mode */
	GPIOA->MODER &= ~GPIO_MODER_MODER14;
	/* set output speed */
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR14;
	/* set default state */
	GPIOA->BSRR = GPIO_BSRR_BR_14;
	/* set as output */
	GPIOA->MODER |= GPIO_MODER_MODER14_0;

	/* default state */
	SR_SetState(0x0000);

	/* exit critical section */
	Critical_Exit();
	/* report status */
	return EOK;
}

/* set shift registeres output */
int SR_SetState(uint16_t state)
{
	/* set default configuration */
	SPI1_Send16b(state);

	/* clock pulse */
	GPIOA->BSRR = GPIO_BSRR_BS_14;
	/* some delay */
	__asm__ volatile("nop\n");
	/* reset clock */
	GPIOA->BSRR = GPIO_BSRR_BR_14;

	/* report status */
	return EOK;
}

