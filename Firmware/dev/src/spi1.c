/*
 * spi1.c
 *
 *  Created on: Aug 28, 2016
 *      Author: Tomek
 */

#include <stm32l1/rcc.h>
#include <stm32l1/gpio.h>
#include <stm32l1/spi.h>
#include <sys/err.h>
#include <sys/critical.h>

/* initialize spi */
int SPI1_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* enable portb */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	/* enable spi1 */
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	/* reset spi1 */
	RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
	/* disable reset */
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;

	/* reset mode */
	GPIOB->MODER &=  ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER5);
	/* set peripheral that will drive the pins */
	GPIOB->AFRL |= (5 << (3*4)) | (5 << (5*4));
	/* set output speed */
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3 | GPIO_OSPEEDER_OSPEEDR5;
	/* set as peripheral driven */
	GPIOB->MODER |= GPIO_MODER_MODER3_1 | GPIO_MODER_MODER5_1;

	/* spi configuration, master mode, internally generated slave select,
	 * 16b format */
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_DFF |
			SPI_CR1_SPE;

	/* exit critical section */
	Critical_Exit();

	/* report status */
	return EOK;
}

/* send 16-bit word over spi1 */
int SPI1_Send16b(uint16_t word)
{
	/* write word */
	SPI1->DR = word;

	/* wait till transfer finishes */
	while (!(SPI1->SR & SPI_SR_TXE));
	/* wait till transfer finishes */
	while (SPI1->SR & SPI_SR_BSY);

	/* report status */
	return EOK;
}
