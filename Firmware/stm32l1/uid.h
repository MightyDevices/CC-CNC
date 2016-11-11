/*
 * uid.h
 *
 *  Created on: 21-11-2012
 *      Author: Tomek
 */

#ifndef STM32L1_UID_H_
#define STM32L1_UID_H_

#include <stm32l1/stm32l1.h>

/* base address */
#define UID_BASE						(0x1FF800D0)
/* base address */
#define UID								((uid_t *)UID_BASE)

/* adcx registers */
typedef struct {
	reg32_t UID0;
	reg32_t UID1;
	reg32_t _RESERVED[3];
	reg32_t UID2;
} uid_t;

#endif /* STM32L1_UID_H_ */
