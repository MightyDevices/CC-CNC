/*
 * comp.h
 *
 *  Created on: 23-01-2014
 *      Author: Tomek
 */

#ifndef STM32L1_COMP_H_
#define STM32L1_COMP_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define COMP_BASE             				(0x40007C00)

/* comparator */
#define COMP								((comp_t *)COMP_BASE)

/* comparator registers */
typedef struct {
	reg32_t CSR;
} __attribute__ ((packed)) comp_t;

/* COMP comparator control and status register */
#define COMP_CSR_10KPU                      (0x00000001)
#define COMP_CSR_400KPU                     (0x00000002)
#define COMP_CSR_10KPD                      (0x00000004)
#define COMP_CSR_400KPD                     (0x00000008)
#define COMP_CSR_CMP1EN                     (0x00000010)
#define COMP_CSR_SW1                        (0x00000020)
#define COMP_CSR_CMP1OUT                    (0x00000080)
#define COMP_CSR_SPEED                      (0x00001000)
#define COMP_CSR_CMP2OUT                    (0x00002000)
#define COMP_CSR_VREFOUTEN                  (0x00010000)
#define COMP_CSR_WNDWE                      (0x00020000)
#define COMP_CSR_INSEL                      (0x001C0000)
#define COMP_CSR_INSEL_0                    (0x00040000)
#define COMP_CSR_INSEL_1                    (0x00080000)
#define COMP_CSR_INSEL_2                    (0x00100000)
#define COMP_CSR_OUTSEL                     (0x00E00000)
#define COMP_CSR_OUTSEL_0                   (0x00200000)
#define COMP_CSR_OUTSEL_1                   (0x00400000)
#define COMP_CSR_OUTSEL_2                   (0x00800000)
#define COMP_CSR_FCH3                       (0x04000000)
#define COMP_CSR_FCH8                       (0x08000000)
#define COMP_CSR_RCH13                      (0x10000000)
#define COMP_CSR_CAIE                       (0x20000000)
#define COMP_CSR_CAIF                       (0x40000000)
#define COMP_CSR_TSUSP                      (0x80000000)


#endif /* STM32L1_COMP_H_ */
