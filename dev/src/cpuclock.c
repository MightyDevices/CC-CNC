/*
 * cpuclock.c
 *
 *  Created on: 16-09-2015
 *      Author: Tomek
 */

#include <dev/cpuclock.h>
#include <stm32l1/rcc.h>
#include <stm32l1/flash.h>
#include <sys/err.h>
#include <sys/ev.h>
#include <sys/critical.h>

/* system clock sources */
#define CLKSRC_MSI							RCC_CFGR_SW_MSI
#define CLKSRC_HSI							RCC_CFGR_SW_HSI
#define CLKSRC_PLL							RCC_CFGR_SW_PLL

/* current frequency */
static volatile int curr_freq = CPUCLOCK_2M096;

/* enable high speed flash mode (for freq > 16MHz)*/
static void CpuClock_FlashHighSpeed(void)
{
	/* 64-bit access */
	FLASH->ACR |= FLASH_ACR_ACC64;
	/* check */
	while ((FLASH->ACR & FLASH_ACR_ACC64) == 0);

	/* insert waitstate */
	FLASH->ACR |= FLASH_ACR_LATENCY;
	/* check */
	while ((FLASH->ACR & FLASH_ACR_LATENCY) == 0);
}

/* set low speed flash operation (for freq <= 16MHz) */
static void CpuClock_FlashLowSpeed(void)
{
	/* no waitstate */
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	/* check */
	while (FLASH->ACR & FLASH_ACR_LATENCY);

	/* 32-bit access */
	FLASH->ACR &= ~FLASH_ACR_ACC64;
	/* check */
	while (FLASH->ACR & FLASH_ACR_ACC64);
}

/* control pll */
static void CpuClock_PLLCmd(int enable)
{
	/* pll on bit */
	bitband_t *pllon = STM32_BB_PERIPH(&RCC->CR, 24);
	/* pll rdy */
	bitband_t *pllrdy = STM32_BB_PERIPH(&RCC->CR, 25);

	/* set state */
	*pllon = enable;
	/* wait till pll settles */
	while ((int)*pllrdy != enable);
}

/* control hsi */
static void CpuClock_HSECmd(int enable)
{
	/* pll on bit */
	bitband_t *hseon = STM32_BB_PERIPH(&RCC->CR, 16);
	/* pll rdy */
	bitband_t *hserdy = STM32_BB_PERIPH(&RCC->CR, 17);

	/* set state */
	*hseon = enable;
	/* wait till hsi settles */
	while ((int)*hserdy != enable);
}

/* set msi range */
static void CpuClock_MSIRange(int range)
{
	/* apply new range */
	RCC->ICSCR = (RCC->ICSCR & ~RCC_ICSCR_MSIRANGE) | (range << 13);
}

/* select system clock source */
static void CpuClock_SelectSource(int source)
{
	/* set new source */
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | source;
	/* wait till it settles */
	while ((int)((RCC->CFGR & RCC_CFGR_SWS) >> 2) != source);
}

/* initialize frequency scaling module */
int CpuClock_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* prepare pll to generate 32MHz from HSI */
	RCC->CFGR |= RCC_CFGR_PLLMUL6 | RCC_CFGR_PLLDIV3 | RCC_CFGR_PLLSRC_HSE;
	/* set starting frequency */
	CpuClock_SetFreq(CPUCLOCK_4M192);

	/* exit critical section */
	Critical_Exit();

	/* report status */
	return EOK;
}

/* get current clock frequency */
int CpuClock_GetFreq(void)
{
	/* report current frequency */
	return curr_freq;
}

/* set frequency */
void CpuClock_SetFreq(int freq)
{
	/* switch on requested frequency */
	switch (freq) {
	/* clocks supplied by msi */
	case CPUCLOCK_1M048 :
	case CPUCLOCK_2M096 :
	case CPUCLOCK_4M192 : {
		/* configure msi range */
		CpuClock_MSIRange(freq);
		/* select msi as clock source */
		CpuClock_SelectSource(CLKSRC_MSI);
		/* disable pll */
		CpuClock_PLLCmd(0);
		/* disable hsi */
		CpuClock_HSECmd(0);
		/* set low speed flash operation */
		CpuClock_FlashLowSpeed();
	} break;
	/* clock supplied by hsi */
	case CPUCLOCK_16M : {
		/* enable hsi */
		CpuClock_HSECmd(1);
		/* select hsi as clock source */
		CpuClock_SelectSource(CLKSRC_HSI);
		/* disable pll */
		CpuClock_PLLCmd(0);
		/* set low speed flash operation */
		CpuClock_FlashLowSpeed();
	} break;
	/* clock supplied by pll */
	case CPUCLOCK_32M : {
		/* set high speed flash operation */
		CpuClock_FlashHighSpeed();
		/* enable hsi */
		CpuClock_HSECmd(1);
		/* enable pll */
		CpuClock_PLLCmd(1);
		/* select hsi as clock source */
		CpuClock_SelectSource(CLKSRC_PLL);
	} break;
	}

	/* assign frequency  */
	curr_freq = freq;
}
