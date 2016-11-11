/*
 * pwr.h
 *
 *  Created on: 01-08-2012
 *      Author: Tomek
 */

#ifndef STM32L1_PWR_H_
#define STM32L1_PWR_H_

#include <stm32l1/stm32l1.h>

/* base */
#define PWR_BASE							(0x40007000)
/* register base */
#define PWR									((pwr_t *) PWR_BASE)

/* registers */
typedef struct {
	reg32_t CR;
	reg32_t CSR;
} pwr_t;

/* PWR power control register */
#define PWR_CR_LPSDSR                       (0x0001)
#define PWR_CR_PDDS                         (0x0002)
#define PWR_CR_CWUF                         (0x0004)
#define PWR_CR_CSBF                         (0x0008)
#define PWR_CR_PVDE                         (0x0010)
#define PWR_CR_PLS                          (0x00E0)
#define PWR_CR_PLS_0                        (0x0020)
#define PWR_CR_PLS_1                        (0x0040)
#define PWR_CR_PLS_2                        (0x0080)
#define PWR_CR_PLS_LEV0                     (0x0000)
#define PWR_CR_PLS_LEV1                     (0x0020)
#define PWR_CR_PLS_LEV2                     (0x0040)
#define PWR_CR_PLS_LEV3                     (0x0060)
#define PWR_CR_PLS_LEV4                     (0x0080)
#define PWR_CR_PLS_LEV5                     (0x00A0)
#define PWR_CR_PLS_LEV6                     (0x00C0)
#define PWR_CR_PLS_LEV7                     (0x00E0)
#define PWR_CR_DBP                          (0x0100)
#define PWR_CR_ULP                          (0x0200)
#define PWR_CR_FWU                          (0x0400)
#define PWR_CR_VOS                          (0x1800)
#define PWR_CR_VOS_0                        (0x0800)
#define PWR_CR_VOS_1                        (0x1000)
#define PWR_CR_LPRUN                        (0x4000)

/* PWR power control/status register */
#define PWR_CSR_WUF                         (0x0001)
#define PWR_CSR_SBF                         (0x0002)
#define PWR_CSR_PVDO                        (0x0004)
#define PWR_CSR_VREFINTRDYF                 (0x0008)
#define PWR_CSR_VOSF                        (0x0010)
#define PWR_CSR_REGLPF                      (0x0020)
#define PWR_CSR_EWUP1                       (0x0100)
#define PWR_CSR_EWUP2                       (0x0200)
#define PWR_CSR_EWUP3                       (0x0400)


#endif /* STM32L1_PWR_H_ */
