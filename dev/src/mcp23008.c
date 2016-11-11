/*
 * mcp23008.c
 *
 *  Created on: Oct 22, 2016
 *      Author: Tomek
 */

#include <sys/err.h>
#include <sys/critical.h>
#include <dev/i2c1.h>
#include <stm32l1/rcc.h>
#include <stm32l1/gpio.h>
#include <stm32l1/exti.h>
#include <stm32l1/nvic.h>
#include <stm32l1/syscfg.h>

#define DEBUG
#include <dev/debug.h>

/* i2c address */
#define MCP23008_ADDR						0x20

/* register addresses */
#define MCP23008_REG_IODIR					0x00
#define MCP23008_REG_IPOL					0x01
#define MCP23008_REG_GPINTEN				0x02
#define MCP23008_REG_DEFVAL					0x03
#define MCP23008_REG_INTCON					0x04
#define MCP23008_REG_IOCON					0x05
#define MCP23008_REG_GPPU					0x06
#define MCP23008_REG_INTF					0x07
#define MCP23008_REG_INTCAP					0x08
#define MCP23008_REG_GPIO					0x09
#define MCP23008_REG_OLAT					0x0A

/* gpio expander event */
ev_t mcp23008_ev;

/* pin state */
static uint8_t pins;

/* i2c job finished */
static void MCP23800_I2CCallback(void *arg)
{
	/* release semaphore */
	Sem_Release(&i2c1_sem);
}

/* interrupt service routine */
void MCP23008_Exti13Isr(void)
{
	/* acquire i2c */
	if (Sem_TryLock(&i2c1_sem) == EOK) {
		/* clear interrupt */
		EXTI->PR = EXTI_PR_PR13;
		/* start transfer */
		I2C1_Transfer(I2C1_READ, MCP23008_ADDR, MCP23008_REG_GPIO, &pins, 1,
				MCP23800_I2CCallback);
	}
}

/* initialize gpio expander */
int MCP23008_Init(void)
{
	/* start-up configuration */
	uint8_t regs[] = {
		/* all pins - inputs, negated, both edge sensitive, interrupt active
		 * high, no pulls */
		0xff, 0xff, 0xff, 0x00, 0x00, 0x02, 0x00
	};

	/* enter critical section */
	Critical_Enter();

	/* enable gpioa */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* configure as input with pull down */
	GPIOA->MODER &= ~GPIO_MODER_MODER13;
	/* enable pull down */
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR13_1;

	/* map pa13 to exti13 */
	SYSCFG->EXTICR4 = (SYSCFG->EXTICR4 & ~SYSCFG_EXTICR4_EXTI13) |
			SYSCFG_EXTICR4_EXTI13_PA;
	/* rising edge sensitivity */
	EXTI->RTSR |= EXTI_RTSR_TR13;
	/* enable interrupt generation */
	EXTI->IMR |= EXTI_IMR_MR13;

	/* exit critical section */
	Critical_Exit();

	/* lock resource */
	Sem_Lock(&i2c1_sem);
	/* perform test transfer */
	I2C1_Transfer(I2C1_WRITE, MCP23008_ADDR, MCP23008_REG_IODIR, regs,
			sizeof(regs), 0);
	/* release resource */
	Sem_Release(&i2c1_sem);

	/* this will force the interrupt to be generated, so that we get initial
	 * pin status */
	EXTI->SWIER = EXTI_SWIER_SWIER13;

	/* report status */
	return EOK;
}

/* return current pin status */
int MCP23008_GetStatus(void)
{
	/* return pins */
	return pins;
}
