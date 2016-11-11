/*
 * opamp.h
 *
 *  Created on: 15-03-2015
 *      Author: Tomek
 */

#ifndef STM32L1_OPAMP_H_
#define STM32L1_OPAMP_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define OPAMP_BASE							(0x40007C5C)

/* base addresses */
#define OPAMP               				((opamp_t *)OPAMP_BASE)

/* opamp registers */
typedef struct {
	reg32_t CSR;
	reg32_t OTR;
	reg32_t LPOTR;
} opamp_t;

/* OPAMP_CSR register */
#define OPAMP_CSR_OPA1PD                   	0x00000001
#define OPAMP_CSR_S3SEL1                    0x00000002
#define OPAMP_CSR_S4SEL1                    0x00000004
#define OPAMP_CSR_S5SEL1                    0x00000008
#define OPAMP_CSR_S6SEL1                    0x00000010
#define OPAMP_CSR_OPA1CAL_L                 0x00000020
#define OPAMP_CSR_OPA1CAL_H                 0x00000040
#define OPAMP_CSR_OPA1LPM                   0x00000080
#define OPAMP_CSR_OPA2PD                    0x00000100
#define OPAMP_CSR_S3SEL2                    0x00000200
#define OPAMP_CSR_S4SEL2                    0x00000400
#define OPAMP_CSR_S5SEL2                    0x00000800
#define OPAMP_CSR_S6SEL2                    0x00001000
#define OPAMP_CSR_OPA2CAL_L                 0x00002000
#define OPAMP_CSR_OPA2CAL_H                 0x00004000
#define OPAMP_CSR_OPA2LPM                   0x00008000
#define OPAMP_CSR_OPA3PD                    0x00010000
#define OPAMP_CSR_S3SEL3                    0x00020000
#define OPAMP_CSR_S4SEL3                    0x00040000
#define OPAMP_CSR_S5SEL3                    0x00080000
#define OPAMP_CSR_S6SEL3                    0x00100000
#define OPAMP_CSR_OPA3CAL_L                 0x00200000
#define OPAMP_CSR_OPA3CAL_H                 0x00400000
#define OPAMP_CSR_OPA3LPM                   0x00800000
#define OPAMP_CSR_ANAWSEL1                  0x01000000
#define OPAMP_CSR_ANAWSEL2                  0x02000000
#define OPAMP_CSR_ANAWSEL3                  0x04000000
#define OPAMP_CSR_S7SEL2                    0x08000000
#define OPAMP_CSR_AOP_RANGE                 0x10000000
#define OPAMP_CSR_OPA1CALOUT                0x20000000
#define OPAMP_CSR_OPA2CALOUT                0x40000000
#define OPAMP_CSR_OPA3CALOUT                0x80000000

/* OPAMP_OTR register */
#define OPAMP_OTR_AO1_OPT_OFFSET_TRIM       0x000003FF
#define OPAMP_OTR_AO2_OPT_OFFSET_TRIM       0x000FFC00
#define OPAMP_OTR_AO3_OPT_OFFSET_TRIM       0x3FF00000
#define OPAMP_OTR_OT_USER                   0x80000000

/* OPAMP_LPOTR register */
#define OPAMP_LP_OTR_AO1_OPT_OFFSET_TRIM_LP 0x000003FF
#define OPAMP_LP_OTR_AO2_OPT_OFFSET_TRIM_LP 0x000FFC00
#define OPAMP_LP_OTR_AO3_OPT_OFFSET_TRIM_LP 0x3FF00000

#endif /* STM32L1_OPAMP_H_ */
