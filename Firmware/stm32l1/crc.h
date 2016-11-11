/*
 * crc.h
 *
 *  Created on: 30-03-2015
 *      Author: Tomek
 */

#ifndef STM32L1_CRC_H_
#define STM32L1_CRC_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define CRC_BASE							(0x40023000)
/* dac registers base address */
#define CRC									((crc_t *)CRC_BASE)

/* dar registers */
typedef struct {
	reg32_t DR;
	reg32_t IDR;
	reg32_t CR;
} __attribute__ ((packed)) crc_t;

/* Control register */
#define CRC_CR_RESET                        0x00000001


#endif /* STM32L1_CRC_H_ */
