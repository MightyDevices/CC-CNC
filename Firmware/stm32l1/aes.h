/*
 * aes.h
 *
 *  Created on: 15-08-2015
 *      Author: Tomek
 */

#ifndef STM32L1_AES_H_
#define STM32L1_AES_H_

/* register base address */
#define AES_BASE              				((uint32_t)0x50060000)
/* register base */
#define AES									((aes_t *)AES_BASE)

/* registers */
typedef struct {
	reg32_t CR;
	reg32_t SR;
	reg32_t DINR;
	reg32_t DOUTR;
	reg32_t KEYR0;
	reg32_t KEYR1;
	reg32_t KEYR2;
	reg32_t KEYR3;
	reg32_t IVR0;
	reg32_t IVR1;
	reg32_t IVR2;
	reg32_t IVR3;
} aes_t;

/* AES control register */
#define AES_CR_EN							((uint32_t)0x00000001)
#define AES_CR_DATATYPE						((uint32_t)0x00000006)
#define AES_CR_DATATYPE_0					((uint32_t)0x00000002)
#define AES_CR_DATATYPE_1					((uint32_t)0x00000004)
#define AES_CR_MODE							((uint32_t)0x00000018)
#define AES_CR_MODE_0						((uint32_t)0x00000008)
#define AES_CR_MODE_1						((uint32_t)0x00000010)
#define AES_CR_CHMOD						((uint32_t)0x00000060)
#define AES_CR_CHMOD_0						((uint32_t)0x00000020)
#define AES_CR_CHMOD_1						((uint32_t)0x00000040)
#define AES_CR_CCFC							((uint32_t)0x00000080)
#define AES_CR_ERRC							((uint32_t)0x00000100)
#define AES_CR_CCIE							((uint32_t)0x00000200)
#define AES_CR_ERRIE						((uint32_t)0x00000400)
#define AES_CR_DMAINEN						((uint32_t)0x00000800)
#define AES_CR_DMAOUTEN						((uint32_t)0x00001000)

/* AES status register */
#define AES_SR_CCF							((uint32_t)0x00000001)
#define AES_SR_RDERR						((uint32_t)0x00000002)
#define AES_SR_WRERR						((uint32_t)0x00000004)

/* AES data input register */
#define AES_DINR							((uint32_t)0x0000FFFF)

/* AES data output register */
#define AES_DOUTR							((uint32_t)0x0000FFFF)

/* AES key register 0 */
#define AES_KEYR0							((uint32_t)0x0000FFFF)

/* AES key register 1 */
#define AES_KEYR1							((uint32_t)0x0000FFFF)

/* AES key register 2 */
#define AES_KEYR2							((uint32_t)0x0000FFFF)

/* AES key register 3 */
#define AES_KEYR3							((uint32_t)0x0000FFFF)

/* AES initialization vector register 0 */
#define AES_IVR0							((uint32_t)0x0000FFFF)

/* AES initialization vector register 1 */
#define AES_IVR1							((uint32_t)0x0000FFFF)

/* AES initialization vector register 2 */
#define AES_IVR2							((uint32_t)0x0000FFFF)

/* AES initialization vector register 3 */
#define AES_IVR3							((uint32_t)0x0000FFFF)

#endif /* STM32L1_AES_H_ */
