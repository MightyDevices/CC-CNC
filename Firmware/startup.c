/*
 * startup.c
 *
 *  Created on: 23-04-2012
 *      Author: Tomek
 */

#include <stdint.h>
#include <stm32l1/stm32l1.h>
#include <stm32l1/rcc.h>
#include <stm32l1/scb.h>
#include <stm32l1/nvic.h>
#include <stm32l1/exti.h>
#include <stm32l1/wwdg.h>
#include <stm32l1/adc.h>
#include <stm32l1/dma.h>
#include <sys/regs.h>
#include <sys/critical.h>

#define DEBUG
#include <dev/debug.h>

/* includes for int/exc handlers */
#include <dev/usart1.h>
#include <dev/watchdog.h>
#include <dev/extimux.h>
#include <dev/usb.h>
#include <dev/led.h>
#include <dev/i2c1.h>
#include <dev/tbase.h>

/* data pointers */
/* bss section */
extern uint32_t __bss_start, __bss_end;
/* initialized data section */
extern uint32_t __data_start, __data_end, __datai_start;
/* exception & interrupt vectors location */
extern uint32_t __flash_vectors;
/* initial stack pointer */
extern uint32_t __stack;

/* main program routine */
extern int main(void);

/* default handler */
void __attribute__((naked)) DefaultHandler(void)
{
#ifdef DEBUG
	/* read stack pointer */
	uint32_t *s = (uint32_t *)Regs_ReadMSP();
	/* read ipsr */
	uint32_t ipsr = Regs_ReadIPSR();
#endif /* DEBUG */

	/* kick wwdg */
	WWDG->CR = WWDG_CR_T;

	/* display warning */
	dprintf_c("default handler reached!\n", 0);
	dprintf_c("msp = 0x%x, ipsr = 0x%x\n", s, ipsr);
	/* display registers */
	dprintf_c("r0 = %x, r1 = %x\n",  s[0], s[1]);
	dprintf_c("r2 = %x, r3 = %x\n",  s[2], s[3]);
	dprintf_c("r12 = %x, lr = %x\n", s[4], s[5]);
	dprintf_c("pc = %x, psr = %x\n", s[6], s[7]);
	/* display active interrupts */
	dprintf_c("ispr0 = %x, ispr1 = %x\n", NVIC->ISPR[0], NVIC->ISPR[1]);
	dprintf_c("iabr0 = %x, iabr1 = %x\n", NVIC->IABR[0], NVIC->IABR[1]);
	/* fault registers */
	dprintf_c("cfsr = %x, hfsr = %x\n", SCB->CFSR, SCB->HFSR);
	dprintf_c("mmar = %x, bfar = %x\n", SCB->MMAR, SCB->BFAR);

	/* reset mcu */
	SCB->AIRCR = SCB_AIRCR_SYSRESETREQ | SCB_AIRCR_VECTKEYSTAT;
}

/* reset handler */
void ResetHandler(void)
{
	/* data initialization pointers */
	volatile uint32_t *start, *end, *starti;

	/* disable interrupts */
	STM32_DISABLEINTS();

	/* disable interrupt generation */
	RCC->CIR = 0;

	/* enable msi */
	RCC->CR |= RCC_CR_MSION;
	/* wait till its enabled */
	while (!(RCC->CR & RCC_CR_MSIRDY));

	/* select msi as a system clock */
	RCC->CFGR = RCC_CFGR_SW_MSI;
	/* wait till its selected */
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI);

	/* select default speed */
	RCC->ICSCR = (RCC->ICSCR & ~RCC_ICSCR_MSIRANGE) | RCC_ICSCR_MSIRANGE_5;

	/* disable all other clocks */
	RCC->CR &= ~(RCC_CR_HSION | RCC_CR_PLLON);
	/* reset pll settings */
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL | RCC_CFGR_PLLDIV);

	/* reset exti interrupts */
	EXTI->IMR = 0;
	/* reset exti events */
	EXTI->EMR = 0;
	/* clear pending interrupts */
	EXTI->PR = 0xffffffff;

	/* disable interrupts 0-31 */
	NVIC->ICER[0] = 0xffffffff;
	/* disable interrupts 32-64 */
	NVIC->ICER[1] = 0xffffffff;
	/* clear pending interrupts: 0-31 */
	NVIC->ICPR[0] = 0xffffffff;
	/* clear pending interrupts: 32-64 */
	NVIC->ICPR[1] = 0xffffffff;

	/* set vector table offset: flash memory */
	SCB->VTOR = (uint32_t)&__flash_vectors;
	/* enable exceptions */
	SCB->SHCSR |= SCB_SHCSR_BUSFATENA | SCB_SHCSR_USGFATENA |
			SCB_SHCSR_MEMFATENA;

	/* copy data section */
	/* data start pointer */
	start = &__data_start;
	/* data end pointer */
	end = &__data_end;
	/* data start (in flash) pointer */
	starti = &__datai_start;

	/* copy */
	while (start < end)
		*start++ = *starti++;

	/* zero bss section */
	/* start of bss section */
	start = &__bss_start;
	/* end of bss section */
	end = &__bss_end;

	/* zero all words */
	while (start < end)
		*start++ = 0;

	/* enable interrupts globally */
	STM32_ENABLEINTS();

	/* jump to main program routine */
	main();

	/* saftery feature, in case main has returned (should never happen) */
	SCB->AIRCR = SCB_AIRCR_SYSRESETREQ | SCB_AIRCR_VECTKEYSTAT;
}


/* init vector table */
uint32_t vectors[] __attribute__ ((section(".vectors"))) = {
	/* initial stack pointer */
	(uint32_t)&__stack,
	/* initial reset handler */
	(uint32_t)ResetHandler,
	/* nmi */
	(uint32_t)DefaultHandler,
	/* hard fault */
	(uint32_t)DefaultHandler,
	/* memmanage */
	(uint32_t)DefaultHandler,
	/* bus fault */
	(uint32_t)DefaultHandler,
	/* usage fault */
	(uint32_t)DefaultHandler,
	/* reserved: firmware size */
	0,
	/* reserved: firmware checksum */
	0,
	/* reserved */
	0, 0,
	/* svc */
	(uint32_t)DefaultHandler,
	/* debug mon */
	(uint32_t)DefaultHandler,
	/* reserved */
	0,
	/* pend sv */
	(uint32_t)DefaultHandler,
	/* systick */
	(uint32_t)DefaultHandler,


	/* Window Watchdog interrupt */
	(uint32_t)DefaultHandler,
	/* PVD through EXTI Line detection interrupt */
	(uint32_t)DefaultHandler,
	/* Tamper and TimeStamp through EXTI line interrupts */
	(uint32_t)DefaultHandler,
	/* RTC Wakeup through EXTI line interrupt */
	(uint32_t)DefaultHandler,
	/* Flash global interrupt */
	(uint32_t)DefaultHandler,
	/* RCC global interrupt */
	(uint32_t)DefaultHandler,
	/* EXTI Line0 interrupt */
	(uint32_t)DefaultHandler,
	/* EXTI Line1 interrupt */
	(uint32_t)DefaultHandler,
	/* EXTI Line2 interrupt */
	(uint32_t)DefaultHandler,
	/* EXTI Line3 interrupt */
	(uint32_t)DefaultHandler,
	/* EXTI Line4 interrupt */
	(uint32_t)DefaultHandler,
	/* DMA1 Channel1 global interrupt */
	(uint32_t)DefaultHandler,
	/* DMA2 Channel1 global interrupt */
	(uint32_t)DefaultHandler,
	/* DMA3 Channel1 global interrupt */
	(uint32_t)DefaultHandler,
	/* DMA4 Channel1 global interrupt */
	(uint32_t)USART1_DMA1Ch4Isr,
	/* DMA5 Channel1 global interrupt */
	(uint32_t)DefaultHandler,
	/* DMA6 Channel1 global interrupt */
	(uint32_t)I2C1_DMA1Ch6Isr,
	/* DMA7 Channel1 global interrupt */
	(uint32_t)I2C1_DMA1Ch7Isr,
	/* ADC1 global interrupt */
	(uint32_t)DefaultHandler,
	/* USB High priority interrupt */
	(uint32_t)DefaultHandler,
	/* USB Low priority interrupt */
	(uint32_t)USB_LPIrq,
	/* DAC interrupt */
	(uint32_t)DefaultHandler,
	/* Comparator wakeup through EXTI line (21 and 22) interrupt */
	(uint32_t)DefaultHandler,
	/* EXTI Line[9:5] interrupts */
	(uint32_t)ExtiMux_Exti5_9Isr,
	/* LCD global interrupt */
	(uint32_t)DefaultHandler,
	/* TIM9 global interrupt */
	(uint32_t)DefaultHandler,
	/* TIM10 global interrupt */
	(uint32_t)Led_Tim10Isr,
	/* TIM11 global interrupt */
	(uint32_t)Led_Tim11Isr,
	/* TIM2 global interrupt */
	(uint32_t)TBase_Tim2Isr,
	/* TIM3 global interrupt */
	(uint32_t)DefaultHandler,
	/* TIM4 global interrupt */
	(uint32_t)DefaultHandler,
	/* I2C1 event interrupt */
	(uint32_t)I2C1_I2C1EvIsr,
	/* I2C1 error interrupt */
	(uint32_t)I2C1_I2C1ErrIsr,
	/* I2C2 event interrupt */
	(uint32_t)DefaultHandler,
	/* I2C2 error interrupt */
	(uint32_t)DefaultHandler,
	/* SPI1 global interrupt */
	(uint32_t)DefaultHandler,
	/* SPI2 global interrupt */
	(uint32_t)DefaultHandler,
	/* USART1 global interrupt */
	(uint32_t)DefaultHandler,
	/* USART2 global interrupt */
	(uint32_t)DefaultHandler,
	/* USART3 global interrupt */
	(uint32_t)DefaultHandler,
	/* EXTI Line[15:10] interrupts */
	(uint32_t)ExtiMux_Exti10_15Isr,
	/* RTC Alarms (A and B) through EXTI line interrupt */
	(uint32_t)DefaultHandler,
	/* USB Device FS Wakeup through EXTI line interrupt */
	(uint32_t)DefaultHandler,
	/* TIM6 global interrupt */
	(uint32_t)DefaultHandler,
	/* TIM7 global interrupt */
	(uint32_t)DefaultHandler,
	/* TIM5 Global interrupt */
	(uint32_t)DefaultHandler,
	/* SPI3 Global interrupt */
	(uint32_t)DefaultHandler,
	/* DMA2 Channel 1 interrupt */
	(uint32_t)DefaultHandler,
	/* DMA2 Channel 2 interrupt */
	(uint32_t)DefaultHandler,
	/* DMA2 Channel 3 interrupt */
	(uint32_t)DefaultHandler,
	/* DMA2 Channel 4 interrupt */
	(uint32_t)DefaultHandler,
	/* DMA2 Channel 5 interrupt */
	(uint32_t)DefaultHandler,
	/* AES global interrupt */
	(uint32_t)DefaultHandler,
	/* Comparator Channel Acquisition Interrupt */
	(uint32_t)DefaultHandler,
};
