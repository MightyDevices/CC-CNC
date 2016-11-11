/*
 * nvic.h
 *
 *  Created on: 14-06-2012
 *      Author: Tomek
 */

#ifndef STM32L1_NVIC_H_
#define STM32L1_NVIC_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define NVIC_BASE							0xE000E100
/* nvic & scb */
#define NVIC								((nvic_t *) NVIC_BASE)

/* nvic registers */
typedef struct {
	reg32_t ISER[8];
	reg32_t RESERVED0[24];
	reg32_t ICER[8];
	reg32_t RSERVED1[24];
	reg32_t ISPR[8];
	reg32_t RESERVED2[24];
	reg32_t ICPR[8];
	reg32_t RESERVED3[24];
	reg32_t IABR[8];
	reg32_t RESERVED4[56];
	reg8_t  IP[240];
	reg32_t RESERVED5[644];
	reg32_t STIR;
} __attribute__ ((packed)) nvic_t;

/* enable interrupt */
#define NVIC_ENABLEINT(i)					\
	(NVIC->ISER[(i) / 32] = 1 << ((i) % 32))

/* disable interrupt */
#define NVIC_DISABLEINT(i)					\
	(NVIC->ICER[(i) / 32] = 1 << ((i) % 32))

/* set interrupt priority */
#define NVIC_SETINTPRI(i, p)				\
	(NVIC->IP[i] = (p))


#endif /* STM32L1_NVIC_H_ */
