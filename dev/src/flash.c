/*
 * flash.c
 *
 *  Created on: 10-06-2014
 *      Author: Tomek
 */

#include <stm32l1/flash.h>
#include <stm32l1/stm32l1.h>
#include <util/size.h>
#include <sys/err.h>
#include <stdint.h>

/* flash unlock */
static void Flash_Unlock(void)
{
	/* already unlocked? */
	if ((FLASH->PECR & FLASH_PECR_PRGLOCK) == 0)
		return;

	/* Unlocking the FLASH_PECR register access */
	if (FLASH->PECR & FLASH_PECR_PELOCK) {
		/* Unlocking the Data memory and FLASH_PECR register access*/
		FLASH->PEKEYR = FLASH_PEKEY1;
		FLASH->PEKEYR = FLASH_PEKEY2;
	}

	/* Unlocking the program memory access */
	FLASH->PRGKEYR = FLASH_PRGKEY1;
	FLASH->PRGKEYR = FLASH_PRGKEY2;
}

/* flash lock */
void Flash_Lock(void)
{
	/* Set the PRGLOCK Bit to lock the program memory access */
	FLASH->PECR |= FLASH_PECR_PRGLOCK;
}

/* flash erase page */
static void Flash_ErasePage(uint32_t page_addr)
{
	/* Wait for last operation to be completed */
	while (FLASH->SR & FLASH_SR_BSY);

	/* Set the ERASE bit */
	FLASH->PECR |= FLASH_PECR_ERASE;
	/* Set PROG bit */
	FLASH->PECR |= FLASH_PECR_PROG;

	/* Write 00000000h to the first word of the program page to erase */
	*(volatile uint32_t *)page_addr = 0x0;

	/* Wait for last operation to be completed */
	while (FLASH->SR & FLASH_SR_BSY);

	/* If the erase operation is completed, disable the ERASE and PROG bits */
	FLASH->PECR &= ~FLASH_PECR_PROG;
	FLASH->PECR &= ~FLASH_PECR_ERASE;
}

/* program half page */
static void RAMFUNC Flash_ProgramHalfPage(uint32_t address, uint32_t *src)
{
	int i;
	/* data pointer */
	uint32_t *p = (uint32_t *)address;

    /* enter programming mode */
    FLASH->PECR |= FLASH_PECR_FPRG;
    FLASH->PECR |= FLASH_PECR_PROG;
    /* wait */
    while (FLASH->SR & FLASH_SR_BSY);

    /* write memory */
    for (i = 0; i < 32; i++)
    	*(p++) = *(src++);

    /* wait */
    while (FLASH->SR & FLASH_SR_BSY);

    /* if the write operation is completed, disable the PROG and FPRG bits */
    FLASH->PECR &= (uint32_t)(~FLASH_PECR_PROG);
    FLASH->PECR &= (uint32_t)(~FLASH_PECR_FPRG);
}

/* initialize flash module */
int Flash_Init(void)
{
	/* report status */
	return EOK;
}

/* initialize flash module */
int Flash_Deinit(void)
{
	/* report status */
	return EOK;
}

/* write page */
void Flash_WritePage(void *dst, void *src)
{
	/* disable interrupts */
	STM32_DISABLEINTS();

	/* unlock flash */
	Flash_Unlock();
	/* erase page */
	Flash_ErasePage((uint32_t)dst);
	/* program 1st half page */
	Flash_ProgramHalfPage((uint32_t)dst + 0x00, (uint32_t *)src + 0x00);
	/* program 2nd half page */
	Flash_ProgramHalfPage((uint32_t)dst + 0x80, (uint32_t *)src + 0x20);
	/* lock flash */
	Flash_Lock();

	/* re-enable interrupts */
	STM32_ENABLEINTS();
}
