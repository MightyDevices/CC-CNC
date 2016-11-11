/*
 * syscfg.h
 *
 *  Created on: 31-03-2013
 *      Author: Tomek
 */

#ifndef STM32L1_SYSCFG_H_
#define STM32L1_SYSCFG_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define SYSCFG_BASE							(0x40010000)
/* registers*/
#define SYSCFG								((syscfg_t *) SYSCFG_BASE)

/* registers */
typedef struct {
	uint32_t MEMRMP;
	uint32_t PMC;
	uint32_t EXTICR1;
	uint32_t EXTICR2;
	uint32_t EXTICR3;
	uint32_t EXTICR4;
} syscfg_t;

/* Bit definition for SYSCFG_MEMRMP register  */
#define SYSCFG_MEMRMP_MEM_MODE          (0x00000003)
#define SYSCFG_MEMRMP_MEM_MODE_0        (0x00000001)
#define SYSCFG_MEMRMP_MEM_MODE_1        (0x00000002)
#define SYSCFG_MEMRMP_BOOT_MODE         (0x00000300)
#define SYSCFG_MEMRMP_BOOT_MODE_0       (0x00000100)
#define SYSCFG_MEMRMP_BOOT_MODE_1       (0x00000200)

/* Bit definition for SYSCFG_PMC register  */
#define SYSCFG_PMC_USB_PU               (0x00000001)

/* Bit definition for SYSCFG_EXTICR1 register */
#define SYSCFG_EXTICR1_EXTI0            (0x000F)
#define SYSCFG_EXTICR1_EXTI1            (0x00F0)
#define SYSCFG_EXTICR1_EXTI2            (0x0F00)
#define SYSCFG_EXTICR1_EXTI3            (0xF000)

/* EXTI0 configuration */
#define SYSCFG_EXTICR1_EXTI0_PA         (0x0000)
#define SYSCFG_EXTICR1_EXTI0_PB         (0x0001)
#define SYSCFG_EXTICR1_EXTI0_PC         (0x0002)
#define SYSCFG_EXTICR1_EXTI0_PD         (0x0003)
#define SYSCFG_EXTICR1_EXTI0_PE         (0x0004)
#define SYSCFG_EXTICR1_EXTI0_PH         (0x0005)
#define SYSCFG_EXTICR1_EXTI0_PF         (0x0006)
#define SYSCFG_EXTICR1_EXTI0_PG         (0x0007)

/* EXTI1 configuration */
#define SYSCFG_EXTICR1_EXTI1_PA         (0x0000)
#define SYSCFG_EXTICR1_EXTI1_PB         (0x0010)
#define SYSCFG_EXTICR1_EXTI1_PC         (0x0020)
#define SYSCFG_EXTICR1_EXTI1_PD         (0x0030)
#define SYSCFG_EXTICR1_EXTI1_PE         (0x0040)
#define SYSCFG_EXTICR1_EXTI1_PH         (0x0050)
#define SYSCFG_EXTICR1_EXTI1_PF         (0x0060)
#define SYSCFG_EXTICR1_EXTI1_PG         (0x0070)

/* EXTI2 configuration */
#define SYSCFG_EXTICR1_EXTI2_PA         (0x0000)
#define SYSCFG_EXTICR1_EXTI2_PB         (0x0100)
#define SYSCFG_EXTICR1_EXTI2_PC         (0x0200)
#define SYSCFG_EXTICR1_EXTI2_PD         (0x0300)
#define SYSCFG_EXTICR1_EXTI2_PE         (0x0400)
#define SYSCFG_EXTICR1_EXTI2_PH         (0x0500)
#define SYSCFG_EXTICR1_EXTI2_PF         (0x0600)
#define SYSCFG_EXTICR1_EXTI2_PG         (0x0700)

/* EXTI3 configuration */
#define SYSCFG_EXTICR1_EXTI3_PA         (0x0000)
#define SYSCFG_EXTICR1_EXTI3_PB         (0x1000)
#define SYSCFG_EXTICR1_EXTI3_PC         (0x2000)
#define SYSCFG_EXTICR1_EXTI3_PD         (0x3000)
#define SYSCFG_EXTICR1_EXTI3_PE         (0x4000)
#define SYSCFG_EXTICR1_EXTI3_PF         (0x3000)
#define SYSCFG_EXTICR1_EXTI3_PG         (0x4000)

/* Bit definition for SYSCFG_EXTICR2 register  */
#define SYSCFG_EXTICR2_EXTI4            (0x000F)
#define SYSCFG_EXTICR2_EXTI5            (0x00F0)
#define SYSCFG_EXTICR2_EXTI6            (0x0F00)
#define SYSCFG_EXTICR2_EXTI7            (0xF000)

/* EXTI4 configuration */
#define SYSCFG_EXTICR2_EXTI4_PA         (0x0000)
#define SYSCFG_EXTICR2_EXTI4_PB         (0x0001)
#define SYSCFG_EXTICR2_EXTI4_PC         (0x0002)
#define SYSCFG_EXTICR2_EXTI4_PD         (0x0003)
#define SYSCFG_EXTICR2_EXTI4_PE         (0x0004)
#define SYSCFG_EXTICR2_EXTI4_PF         (0x0006)
#define SYSCFG_EXTICR2_EXTI4_PG         (0x0007)

/* EXTI5 configuration */
#define SYSCFG_EXTICR2_EXTI5_PA         (0x0000)
#define SYSCFG_EXTICR2_EXTI5_PB         (0x0010)
#define SYSCFG_EXTICR2_EXTI5_PC         (0x0020)
#define SYSCFG_EXTICR2_EXTI5_PD         (0x0030)
#define SYSCFG_EXTICR2_EXTI5_PE         (0x0040)
#define SYSCFG_EXTICR2_EXTI5_PF         (0x0060)
#define SYSCFG_EXTICR2_EXTI5_PG         (0x0070)

/* EXTI6 configuration */
#define SYSCFG_EXTICR2_EXTI6_PA         (0x0000)
#define SYSCFG_EXTICR2_EXTI6_PB         (0x0100)
#define SYSCFG_EXTICR2_EXTI6_PC         (0x0200)
#define SYSCFG_EXTICR2_EXTI6_PD         (0x0300)
#define SYSCFG_EXTICR2_EXTI6_PE         (0x0400)
#define SYSCFG_EXTICR2_EXTI6_PF         (0x0600)
#define SYSCFG_EXTICR2_EXTI6_PG         (0x0700)

/* EXTI7 configuration */
#define SYSCFG_EXTICR2_EXTI7_PA         (0x0000)
#define SYSCFG_EXTICR2_EXTI7_PB         (0x1000)
#define SYSCFG_EXTICR2_EXTI7_PC         (0x2000)
#define SYSCFG_EXTICR2_EXTI7_PD         (0x3000)
#define SYSCFG_EXTICR2_EXTI7_PE         (0x4000)
#define SYSCFG_EXTICR2_EXTI7_PF         (0x6000)
#define SYSCFG_EXTICR2_EXTI7_PG         (0x7000)

/* Bit definition for SYSCFG_EXTICR3 register  */
#define SYSCFG_EXTICR3_EXTI8            (0x000F)
#define SYSCFG_EXTICR3_EXTI9            (0x00F0)
#define SYSCFG_EXTICR3_EXTI10           (0x0F00)
#define SYSCFG_EXTICR3_EXTI11           (0xF000)

/* EXTI8 configuration */
#define SYSCFG_EXTICR3_EXTI8_PA         (0x0000)
#define SYSCFG_EXTICR3_EXTI8_PB         (0x0001)
#define SYSCFG_EXTICR3_EXTI8_PC         (0x0002)
#define SYSCFG_EXTICR3_EXTI8_PD         (0x0003)
#define SYSCFG_EXTICR3_EXTI8_PE         (0x0004)
#define SYSCFG_EXTICR3_EXTI8_PF         (0x0006)
#define SYSCFG_EXTICR3_EXTI8_PG         (0x0007)

/* EXTI9 configuration */
#define SYSCFG_EXTICR3_EXTI9_PA         (0x0000)
#define SYSCFG_EXTICR3_EXTI9_PB         (0x0010)
#define SYSCFG_EXTICR3_EXTI9_PC         (0x0020)
#define SYSCFG_EXTICR3_EXTI9_PD         (0x0030)
#define SYSCFG_EXTICR3_EXTI9_PE         (0x0040)
#define SYSCFG_EXTICR3_EXTI9_PF         (0x0060)
#define SYSCFG_EXTICR3_EXTI9_PG         (0x0070)

/* EXTI10 configuration */
#define SYSCFG_EXTICR3_EXTI10_PA        (0x0000)
#define SYSCFG_EXTICR3_EXTI10_PB        (0x0100)
#define SYSCFG_EXTICR3_EXTI10_PC        (0x0200)
#define SYSCFG_EXTICR3_EXTI10_PD        (0x0300)
#define SYSCFG_EXTICR3_EXTI10_PE        (0x0400)
#define SYSCFG_EXTICR3_EXTI10_PF        (0x0600)
#define SYSCFG_EXTICR3_EXTI10_PG        (0x0700)

/* EXTI11 configuration */
#define SYSCFG_EXTICR3_EXTI11_PA        (0x0000)
#define SYSCFG_EXTICR3_EXTI11_PB        (0x1000)
#define SYSCFG_EXTICR3_EXTI11_PC        (0x2000)
#define SYSCFG_EXTICR3_EXTI11_PD        (0x3000)
#define SYSCFG_EXTICR3_EXTI11_PE        (0x4000)
#define SYSCFG_EXTICR3_EXTI11_PF        (0x6000)
#define SYSCFG_EXTICR3_EXTI11_PG        (0x7000)

/* Bit definition for SYSCFG_EXTICR4 register  */
#define SYSCFG_EXTICR4_EXTI12           (0x000F)
#define SYSCFG_EXTICR4_EXTI13           (0x00F0)
#define SYSCFG_EXTICR4_EXTI14           (0x0F00)
#define SYSCFG_EXTICR4_EXTI15           (0xF000)

/* EXTI12 configuration */
#define SYSCFG_EXTICR4_EXTI12_PA        (0x0000)
#define SYSCFG_EXTICR4_EXTI12_PB        (0x0001)
#define SYSCFG_EXTICR4_EXTI12_PC        (0x0002)
#define SYSCFG_EXTICR4_EXTI12_PD        (0x0003)
#define SYSCFG_EXTICR4_EXTI12_PE        (0x0004)
#define SYSCFG_EXTICR4_EXTI12_PF        (0x0006)
#define SYSCFG_EXTICR4_EXTI12_PG        (0x0007)

/* EXTI13 configuration */
#define SYSCFG_EXTICR4_EXTI13_PA        (0x0000)
#define SYSCFG_EXTICR4_EXTI13_PB        (0x0010)
#define SYSCFG_EXTICR4_EXTI13_PC        (0x0020)
#define SYSCFG_EXTICR4_EXTI13_PD        (0x0030)
#define SYSCFG_EXTICR4_EXTI13_PE        (0x0040)
#define SYSCFG_EXTICR4_EXTI13_PF        (0x0060)
#define SYSCFG_EXTICR4_EXTI13_PG        (0x0070)

/* EXTI14 configuration */
#define SYSCFG_EXTICR4_EXTI14_PA        (0x0000)
#define SYSCFG_EXTICR4_EXTI14_PB        (0x0100)
#define SYSCFG_EXTICR4_EXTI14_PC        (0x0200)
#define SYSCFG_EXTICR4_EXTI14_PD        (0x0300)
#define SYSCFG_EXTICR4_EXTI14_PE        (0x0400)
#define SYSCFG_EXTICR4_EXTI14_PF        (0x0600)
#define SYSCFG_EXTICR4_EXTI14_PG        (0x0700)

/* EXTI15 configuration */
#define SYSCFG_EXTICR4_EXTI15_PA        (0x0000)
#define SYSCFG_EXTICR4_EXTI15_PB        (0x1000)
#define SYSCFG_EXTICR4_EXTI15_PC        (0x2000)
#define SYSCFG_EXTICR4_EXTI15_PD        (0x3000)
#define SYSCFG_EXTICR4_EXTI15_PE        (0x4000)
#define SYSCFG_EXTICR4_EXTI15_PF        (0x6000)
#define SYSCFG_EXTICR4_EXTI15_PG        (0x7000)

#endif /* STM32L1_SYSCFG_H_ */
