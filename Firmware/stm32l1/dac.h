/*
 * dac.h
 *
 *  Created on: 29-09-2013
 *      Author: Tomek
 */

#ifndef STM32L1_DAC_H_
#define STM32L1_DAC_H_

#include <stm32l1/stm32l1.h>

/* dac register base */
#define DAC_BASE							(0x40007400)
/* dac registers base address */
#define DAC									((dac_t *)DAC_BASE)

/* dar registers */
typedef struct {
	reg32_t CR;
	reg32_t SWTRIGR;
	reg32_t DHR12R1;
	reg32_t DHR12L1;
	reg32_t DHR8R1;
	reg32_t DHR12R2;
	reg32_t DHR12L2;
	reg32_t DHR8R2;
	reg32_t DHR12RD;
	reg32_t DHR12LD;
	reg32_t DHR8RD;
	reg32_t DOR1;
	reg32_t DOR2;
	reg32_t SR;
} __attribute__ ((packed)) dac_t;

/* DAC_CR register */
#define DAC_CR_EN1                         	(0x00000001)
#define DAC_CR_BOFF1                       	(0x00000002)
#define DAC_CR_TEN1                        	(0x00000004)
#define DAC_CR_TSEL1                       	(0x00000038)
#define DAC_CR_TSEL1_0                     	(0x00000008)
#define DAC_CR_TSEL1_1                     	(0x00000010)
#define DAC_CR_TSEL1_2                     	(0x00000020)
#define DAC_CR_WAVE1                       	(0x000000C0)
#define DAC_CR_WAVE1_0                     	(0x00000040)
#define DAC_CR_WAVE1_1                     	(0x00000080)
#define DAC_CR_MAMP1                       	(0x00000F00)
#define DAC_CR_MAMP1_0                     	(0x00000100)
#define DAC_CR_MAMP1_1                     	(0x00000200)
#define DAC_CR_MAMP1_2                     	(0x00000400)
#define DAC_CR_MAMP1_3                     	(0x00000800)
#define DAC_CR_DMAEN1                      	(0x00001000)
#define DAC_CR_EN2                         	(0x00010000)
#define DAC_CR_BOFF2                       	(0x00020000)
#define DAC_CR_TEN2                        	(0x00040000)
#define DAC_CR_TSEL2                       	(0x00380000)
#define DAC_CR_TSEL2_0                     	(0x00080000)
#define DAC_CR_TSEL2_1                     	(0x00100000)
#define DAC_CR_TSEL2_2                     	(0x00200000)
#define DAC_CR_WAVE2                       	(0x00C00000)
#define DAC_CR_WAVE2_0                     	(0x00400000)
#define DAC_CR_WAVE2_1                     	(0x00800000)
#define DAC_CR_MAMP2                       	(0x0F000000)
#define DAC_CR_MAMP2_0                     	(0x01000000)
#define DAC_CR_MAMP2_1                     	(0x02000000)
#define DAC_CR_MAMP2_2                     	(0x04000000)
#define DAC_CR_MAMP2_3                     	(0x08000000)
#define DAC_CR_DMAEN2                      	(0x10000000)

/* DAC_SWTRIGR register */
#define DAC_SWTRIGR_SWTRIG1                	(0x01)
#define DAC_SWTRIGR_SWTRIG2                	(0x02)

/* DAC_DHR12R1 register */
#define DAC_DHR12R1_DACC1DHR               	(0x0FFF)

/* DAC_DHR12L1 register */
#define DAC_DHR12L1_DACC1DHR               	(0xFFF0)

/* DAC_DHR8R1 register */
#define DAC_DHR8R1_DACC1DHR                	(0xFF)

/* DAC_DHR12R2 register */
#define DAC_DHR12R2_DACC2DHR               	(0x0FFF)

/* DAC_DHR12L2 register */
#define DAC_DHR12L2_DACC2DHR               	(0xFFF0)

/* DAC_DHR8R2 register */
#define DAC_DHR8R2_DACC2DHR                	(0xFF)

/* DAC_DHR12RD register */
#define DAC_DHR12RD_DACC1DHR               	(0x00000FFF)
#define DAC_DHR12RD_DACC2DHR               	(0x0FFF0000)

/* DAC_DHR12LD register */
#define DAC_DHR12LD_DACC1DHR               	(0x0000FFF0)
#define DAC_DHR12LD_DACC2DHR               	(0xFFF00000)

/*  DAC_DHR8RD register **/
#define DAC_DHR8RD_DACC1DHR                	(0x00FF)
#define DAC_DHR8RD_DACC2DHR                	(0xFF00)

/* DAC_DOR1 register */
#define DAC_DOR1_DACC1DOR                  	(0x0FFF)

/* DAC_DOR2 register */
#define DAC_DOR2_DACC2DOR                  	(0x0FFF)

/* DAC_SR register */
#define DAC_SR_DMAUDR1                     	(0x00002000)
#define DAC_SR_DMAUDR2                     	(0x20000000)

#endif /* STM32L1_DAC_H_ */
