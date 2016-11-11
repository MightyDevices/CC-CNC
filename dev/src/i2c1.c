/*
 * i2c1.c
 *
 *  Created on: 02-07-2015
 *      Author: Tomek
 */


#include <stm32l1/rcc.h>
#include <stm32l1/gpio.h>
#include <stm32l1/i2c.h>
#include <stm32l1/dma.h>
#include <stm32l1/nvic.h>
#include <stm32l1/stm32l1.h>
#include <dev/i2c1.h>
#include <sys/cb.h>
#include <sys/err.h>
#include <sys/ev.h>
#include <sys/sem.h>
#include <sys/idle.h>
#include <sys/mods.h>
#include <sys/critical.h>
#include <util/size.h>

#define DEBUG
#include <dev/debug.h>

/* transfer sem */
sem_t i2c1_sem;
/* transfer callback */
static cb_t callback;

/* operation type operation errors */
static volatile int type, error;
/* current address and sub address */
static volatile uint8_t address, sub_address;
/* operation pointer */
static void *o_ptr;
/* operation size */
static size_t o_size;

/* end transfdfer*/
static void I2C1_EndTransfer(int rc)
{
	/* call callback if not empty or null */
	if (callback != CB_NULL) {
		callback((void *)&rc);
	/* change the address to notify sync caller that operation has ended */
	} else {
		callback = CB_NONE;
	}
}

/* event interrupt */
void I2C1_I2C1EvIsr(void)
{
	/* read status register */
	uint16_t sr = I2C1->SR1;

	/* start condition emitted? */
	if (sr & I2C_SR1_SB) {
		/* send address byte */
		I2C1->DR = address;
	/* address stage */
	} else if (sr & I2C_SR1_ADDR) {
		/* automatic ack generation for long reads */
		if (o_size > 1)
			I2C1->CR1 |= I2C_CR1_ACK;
		/* this dummy read will start the data phase */
		I2C1->SR2;
	}
}

/* error interrupt routine */
void I2C1_I2C1ErrIsr(void)
{
	/* read status */
	error = I2C1->SR1 & (I2C_SR1_AF | I2C_SR1_ARLO | I2C_SR1_BERR);
	/* clear flags */
	I2C1->SR1 = ~error;

	/* in case of bit error or ack failure software must abort the
	 * transmission */
	if (error & (I2C_SR1_AF | I2C_SR1_BERR)) {
		/* generate stop condition */
		I2C1->CR1 |= I2C_CR1_STOP;
		/* wait till its stopped */
		while (I2C1->CR1 & I2C_CR1_STOP);
	}
	/* disable i2c */
	I2C1->CR1 &= ~I2C_CR1_PE;
	/* end transfer */
	I2C1_EndTransfer(error);
}

/* rx dma routine */
void I2C1_DMA1Ch7Isr(void)
{
	/* clear flag */
	DMA1->IFCR = DMA_IFCR_CGIF7;

	/* stop transfer */
	I2C1->CR1 |= I2C_CR1_STOP;
	/* wait till its stopped */
	while (!error && (I2C1->CR1 & I2C_CR1_STOP));
	/* disable i2c */
	I2C1->CR1 &= ~I2C_CR1_PE;

	/* end transfer */
	I2C1_EndTransfer(error);
}

/* tx dma routine */
void I2C1_DMA1Ch6Isr(void)
{
	dma_chan_t *dma;

	/* clear flag */
	DMA1->IFCR = DMA_IFCR_CGIF6;
	/* choose proper dma channel */
	dma = type == I2C1_READ ? DMA1C7 : DMA1C6;

	/* operation ongoing? */
	if (type == I2C1_READ || type == I2C1_WRITE) {
		/* clear type */
		type = -1;

		/* disable dma */
		dma->CCR &= ~DMA_CCR_EN;
		/* set memory pointer */
		dma->CMAR = (uint32_t)o_ptr;
		/* set number of bytes */
		dma->CNDTR = o_size;
		/* re-enable dma */
		dma->CCR |= DMA_CCR_EN;

		/* got read operation? */
		if (dma == DMA1C7) {
			/* set 'read' bit */
			address |= 1;
			/* generate repeated start condition */
			I2C1->CR1 |= I2C_CR1_START;
		}
	/* end of tx */
	} else {
		/* wait for end of byte sending */
		while (!error && (I2C1->SR1 & I2C_SR1_BTF) == 0);
		/* stop transfer */
		I2C1->CR1 |= I2C_CR1_STOP;
		/* wait till its stopped */
		while (!error && (I2C1->CR1 & I2C_CR1_STOP));
		/* disable i2c */
		I2C1->CR1 &= ~I2C_CR1_PE;

		/* end transfer */
		I2C1_EndTransfer(error);
	}
}

/* initialize i2c1 */
int I2C1_Init(void)
{
	/* disable interrupts to ensure atomicity of register access */
	Critical_Enter();

	/* enable gpio b */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_DMA1EN;
	/* enable i2c2 csem */
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	/* enable reset */
	RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	/* disable reset */
	RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

	/* prepare pins */
	/* reset function */
	GPIOB->AFRH &= ~(0xf << 0 | 0xf << 4);
	/* set alternate function */
	GPIOB->AFRH |= 4 << 0 | 4 << 4;
	/* open drain output */
	GPIOB->OTYPER |= GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9;
	/* output speed */
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9;
	/* clear out moder */
	GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);


	/* BUGFIX: i2c slaves can block sda when reset occurs in the middle of the
	 * transmission. In order to restore normal operation master has to clock
	 * some pulses till slave releases sda. */
	/* set clock line as output */
	GPIOB->MODER |= GPIO_MODER_MODER8_0;
	/* clock pulses till sda is idle */
	for (int i = 0; i < 32; i++) {
		/* clock toggle */
		GPIOB->BSRR = GPIO_BSRR_BR_8;
		/* delay */
		for (int j = 0; j < 16; j++)
			__asm__ volatile ("nop\n");
		/* this is going to be delayed by the loop itself */
		GPIOB->BSRR = GPIO_BSRR_BS_8;
		/* delay */
		for (int j = 0; j < 16; j++)
			__asm__ volatile ("nop\n");
	}
	/* release clock drive */
	GPIOB->MODER &= ~GPIO_MODER_MODER8_0;

	/* set alternate function */
	GPIOB->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;

	/* prepare i2c */
	/* set frequency, enable dma transfer, enable error & event interrupt */
	I2C1->CR2 = I2C_CR2_DMAEN | I2C_CR2_LAST | I2C_CR2_ITERREN |
			I2C_CR2_ITEVTEN | 0x04;
	/* scl maximal rise time */
	I2C1->TRISE = 10;
	/* 100kHz csem */
	I2C1->CCR = 20;

	/* prepare tx dma */
	DMA1C6->CPAR = (uint32_t)&I2C1->DR;
	/* configure channel */
	DMA1C6->CCR = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;

	/* prepare rx dma */
	DMA1C7->CPAR = (uint32_t)&I2C1->DR;
	/* configure channel */
	DMA1C7->CCR = DMA_CCR_MINC | DMA_CCR_TCIE;

	/* set priorities */
	NVIC_SETINTPRI(STM32_INT_I2C1ER, 0x10);
	NVIC_SETINTPRI(STM32_INT_I2C1EV, 0x20);
	NVIC_SETINTPRI(STM32_INT_DMA1_CH6, 0x20);
	NVIC_SETINTPRI(STM32_INT_DMA1_CH7, 0x20);

	/* enable all interrupts */
	NVIC_ENABLEINT(STM32_INT_I2C1ER);
	NVIC_ENABLEINT(STM32_INT_I2C1EV);
	NVIC_ENABLEINT(STM32_INT_DMA1_CH6);
	NVIC_ENABLEINT(STM32_INT_DMA1_CH7);

	/* restore interrupts */
	Critical_Exit();

	/* reset sem */
	Sem_Release(&i2c1_sem);

	/* report status */
	return EOK;
}

/* i2c transfer */
int I2C1_Transfer(int oper_type, uint8_t addr, uint8_t sub_addr, void *ptr,
		size_t size, cb_t cb)
{
	/* result code, sync or async call? */
	int rc = size, sync = cb == CB_NULL;

	/* store callback */
	callback = cb;

	/* set operation type, reset errors */
	type = oper_type, error = 0;
	/* store addresses */
	address = addr << 1, sub_address = sub_addr;
	/* set data pointer and data size */
	o_ptr = ptr, o_size = size;

	/* disable dma */
	DMA1C6->CCR &= ~DMA_CCR_EN;
	/* set memory to sub address */
	DMA1C6->CMAR = (uint32_t)&sub_address;
	/* one byte of subaddress will be transfered */
	DMA1C6->CNDTR = 1;
	/* re-enable dma */
	DMA1C6->CCR |= DMA_CCR_EN;

	/* enable peripheral and send a start bit */
	I2C1->CR1 = I2C_CR1_PE | I2C_CR1_START;

	/* sync call? */
	if (sync) {
		/* interrupt will alter cb_tx address after transfer has finished */
		while (callback == CB_NULL);
		/* got error? */
		if (error)
			/* update result code */
			rc = -error;
	}

	/* report status */
	return rc;
}
