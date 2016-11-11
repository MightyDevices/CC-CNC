/*
 * iwdg.h
 *
 *  Created on: 24-02-2013
 *      Author: Tomek
 */

#ifndef STM32L1_IWDG_H_
#define STM32L1_IWDG_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define IWDG_BASE		            		(0x40003000)
/* registers */
#define IWDG              					((iwdg_t *) IWDG_BASE)

/* registers */
typedef struct {
	reg32_t KR;
	reg32_t PR;
	reg32_t RLR;
	reg32_t SR;
} __attribute__ ((packed)) iwdg_t;

/* Defines for watchdog operaitons */
#define IWDG_KR_KEY_RELOAD    				(0xAAAA)
#define IWDG_KR_KEY_ENABLE    				(0xCCCC)

/* Enable / disable IWDG config registers access */
#define IWDG_WRITEACCESS_ENABLE				(0x5555)
#define IWDG_WRITEACCESS_DISABLE    		(0x0000)

/* status register */
#define IWDG_SR_PVU							(0x00000001)
#define IWDG_SR_RVU							(0x00000002)


#endif /* STM32L1_IWDG_H_ */
