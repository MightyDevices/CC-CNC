/*
 * drv8824.c
 *
 *  Created on: Aug 28, 2016
 *      Author: Tomek
 */

#include <stm32l1/rcc.h>
#include <stm32l1/gpio.h>
#include <sys/err.h>
#include <sys/critical.h>
#include <dev/sr.h>
#include <dev/drv8824.h>
#include <dev/mcp4728.h>

/* initialize stepper motor drivers */
int DRV8824_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* enable gpios */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;

	/* configure gpioa */
	GPIOA->MODER &=  ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 |
			GPIO_MODER_MODER2 | GPIO_MODER_MODER3 | GPIO_MODER_MODER4 |
			GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7 |
			GPIO_MODER_MODER8);
	/* set default output levels */
	GPIOA->BSRR = GPIO_BSRR_BR_0 | GPIO_BSRR_BR_1 | GPIO_BSRR_BR_4 |
			GPIO_BSRR_BR_5;
	/* set pull ups on input pins */
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_0 | GPIO_PUPDR_PUPDR7_0 |
			GPIO_PUPDR_PUPDR8_0;
	/* enable outputs */
	GPIOA->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 |
			GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0;


	/* configure gpiob */
	GPIOB->MODER &=  ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 |
			GPIO_MODER_MODER2 | GPIO_MODER_MODER10 | GPIO_MODER_MODER12 |
			GPIO_MODER_MODER13 | GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
	/* set default output levels */
	GPIOB->BSRR = GPIO_BSRR_BR_1 | GPIO_BSRR_BR_2 | GPIO_BSRR_BR_12 |
			GPIO_BSRR_BR_13 | GPIO_BSRR_BR_14;
	/* set pull ups on input pins */
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR10_0;
	/* enable outputs */
	GPIOB->MODER |= GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 |
			GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0;

	/* configure slow decay */
	GPIOA->BSRR = GPIO_BSRR_BR_2 | GPIO_BSRR_BR_6;
	GPIOB->BSRR = GPIO_BSRR_BR_0 | GPIO_BSRR_BR_15;
	/* enable slow decay */
	GPIOA->MODER |= GPIO_MODER_MODER2_0 | GPIO_MODER_MODER6_0;
	GPIOB->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER15_0;

	/* default configuration: enable devices, 1/8 step mode */
	SR_SetState(SR_A_MD0 | SR_A_MD1 | SR_X_MD0 | SR_X_MD1 | SR_Y_MD0 |
			SR_Y_MD1 | SR_Z_MD0 | SR_Z_MD1);

	/* exit critical section */
	Critical_Exit();

	/* set current limit */
//	uint16_t vals[4] = {1000, 1000, 1000, 1000};
	uint16_t vals[4] = {600, 600, 600, 600};
	/* apply limits */
	MCP4728_SetChannels(vals, 0);

	/* disable reset */
	GPIOB->BSRR = GPIO_BSRR_BS_2;

	/* report status */
	return EOK;
}

/* enable/disable motors power */
int DRV8824_MotorsCmd(int enable)
{
	/* disable motors */
	if (!enable) {
		SR_SetState(SR_A_ENBL | SR_Z_ENBL | SR_Y_ENBL | SR_Z_ENBL);
	/* enable */
	} else {
		SR_SetState(SR_A_MD0 | SR_A_MD1 | SR_X_MD0 | SR_X_MD1 | SR_Y_MD0 |
				SR_Y_MD1 | SR_Z_MD0 | SR_Z_MD1);
	}

	/* report status */
	return EOK;
}

/* perform a single step */
int DRV8824_Step(uint32_t step, uint32_t dir)
{
	/* gpio bsrrs */
	uint32_t dir_a = GPIO_BSRR_BR_1 | GPIO_BSRR_BR_5;
	uint32_t dir_b = GPIO_BSRR_BR_1 | GPIO_BSRR_BR_14;
	uint32_t step_a = 0, step_b = 0;

	/* prepare dir */
	if (dir & DRV8824_X) dir_a |= GPIO_BSRR_BS_1;
	if (dir & DRV8824_Y) dir_a |= GPIO_BSRR_BS_5;
	if (dir & DRV8824_Z) dir_b |= GPIO_BSRR_BS_1;
	if (dir & DRV8824_A) dir_b |= GPIO_BSRR_BS_14;

	/* clear out reset bits */
	dir_a = dir_a ^ (dir_a << 16);
	dir_b = dir_b ^ (dir_b << 16);

	/* apply dirs */
	GPIOA->BSRR = dir_a, GPIOB->BSRR = dir_b;

	/* prepare step */
	if (step & DRV8824_X) step_a |= GPIO_BSRR_BS_0;
	if (step & DRV8824_Y) step_a |= GPIO_BSRR_BS_4;
	if (step & DRV8824_Z) step_b |= GPIO_BSRR_BS_12;
	if (step & DRV8824_A) step_b |= GPIO_BSRR_BS_13;

	/* apply steps */
	GPIOA->BSRR = step_a, GPIOB->BSRR = step_b;

	/* report status */
	return EOK;
}

/* reset step outputs (needed before next step) */
void DRV8824_StepReset(void)
{
	/* reset step pins */
	GPIOA->BSRR = GPIO_BSRR_BR_0 | GPIO_BSRR_BR_4;
	GPIOB->BSRR = GPIO_BSRR_BR_12 | GPIO_BSRR_BR_13;
}

