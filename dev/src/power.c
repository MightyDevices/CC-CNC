/*
 * pvd.c
 *
 *  Created on: 13-04-2014
 *      Author: Tomek
 */

#include <stm32l1/pwr.h>
#include <stm32l1/rcc.h>
#include <stm32l1/scb.h>
#include <stm32l1/exti.h>
#include <stm32l1/syscfg.h>
#include <stm32l1/gpio.h>
#include <stm32l1/nvic.h>
#include <dev/watchdog.h>
#include <dev/power.h>
#include <sys/err.h>
#include <sys/critical.h>
#include <stdint.h>

#define DEBUG
#include <dev/debug.h>

/* power level detector init */
int Power_Init(void)
{
	/* disable interrupts to ensure atomicity of register access */
	Critical_Enter();

	/* enable power */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	/* enable all gpios for configuring before low power mode */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN |
			RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOHEN;

	RCC->AHBRSTR |= RCC_AHBRSTR_GPIOARST | RCC_AHBRSTR_GPIOBRST |
			RCC_AHBRSTR_GPIOCRST | RCC_AHBRSTR_GPIOHRST;
	RCC->AHBRSTR &= ~(RCC_AHBRSTR_GPIOARST | RCC_AHBRSTR_GPIOBRST |
			RCC_AHBRSTR_GPIOCRST | RCC_AHBRSTR_GPIOHRST);

	/* reset usart */
	RCC->APB1RSTR |= RCC_APB1RSTR_PWRRST;
	/* disable reset */
	RCC->APB1RSTR &= ~RCC_APB1RSTR_PWRRST;

	/* set outputs for lowest consumption */
	GPIOA->ODR = 0x0000; GPIOB->ODR = 0x0000;
	/* set outputs for lowest consumption */
	GPIOC->ODR = 0x0000;

	/* set for lowest consumption */
	GPIOA->MODER = 0xffffffff; GPIOB->MODER = 0xffffffff;
	/* set for lowest consumption */
	GPIOC->MODER = 0xffffffff; GPIOH->MODER = 0xffffffff;

	/* pull down configuration (ports a and b) */
	GPIOA->PUPDR = 0x00000000; GPIOB->PUPDR = 0x00000000;
	/* pull down configuration (ports a and b) */
	GPIOC->PUPDR = 0x00000000; GPIOH->PUPDR = 0x00000000;

	/* normal operation (1.2v vcore, voltage detector enable) */
	PWR->CR = PWR_CR_VOS_0;
	/* reset wake up pin functionality */
	PWR->CSR = 0;

	/* restore interrupts */
	Critical_Exit();

	/* report status */
	return EOK;
}

/* sleep mode */
void Power_SleepMode(void)
{
	/* go to sleep, wait for wakeup event */
	__asm__ volatile ("wfi");
}

/* enter stop mode, exit on exti event */
void Power_StopMode(void)
{
	/* clear wakeup flag, enter ultra low power mode */
	PWR->CR |= PWR_CR_CWUF | PWR_CR_ULP | PWR_CR_LPSDSR | PWR_CR_FWU;
	/* clear standby mode */
	PWR->CR &= ~PWR_CR_PDDS;

	/* Set SLEEPDEEP bit of Cortex System Control Register */
	SCB->SCR |= SCB_SCR_SLEEPDEEP;

	/* go to sleep, wait for wakeup event */
	__asm__ volatile ("wfi");

	/* reset deep sleep bit */
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP;
	/* reset ultra low power run mode */
	PWR->CR &= ~PWR_CR_LPSDSR;
}
