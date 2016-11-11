/*
 * i2c.h
 *
 *  Created on: 21-11-2012
 *      Author: Tomek
 */

#ifndef STM32L1_I2C_H_
#define STM32L1_I2C_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define I2C1_BASE							(0x40005400)
#define I2C2_BASE							(0x40005800)

/* usart */
#define I2C1              					((i2c_t *) I2C1_BASE)
#define I2C2              					((i2c_t *) I2C2_BASE)

/* registers */
typedef struct {
	reg16_t 	CR1;
	reg16_t  	RESERVED0;
	reg16_t 	CR2;
	reg16_t  	RESERVED1;
	reg16_t 	OAR1;
	reg16_t  	RESERVED2;
	reg16_t 	OAR2;
	reg16_t  	RESERVED3;
	reg16_t 	DR;
	reg16_t  	RESERVED4;
	reg16_t 	SR1;
	reg16_t  	RESERVED5;
	reg16_t 	SR2;
	reg16_t  	RESERVED6;
	reg16_t 	CCR;
	reg16_t  	RESERVED7;
	reg16_t 	TRISE;
	reg16_t  	RESERVED8;
} __attribute__((packed)) i2c_t;

/* I2C Control register 1 */
#define I2C_CR1_PE                          (0x0001)
#define I2C_CR1_SMBUS                       (0x0002)
#define I2C_CR1_SMBTYPE                     (0x0008)
#define I2C_CR1_ENARP                       (0x0010)
#define I2C_CR1_ENPEC                       (0x0020)
#define I2C_CR1_ENGC                        (0x0040)
#define I2C_CR1_NOSTRETCH                   (0x0080)
#define I2C_CR1_START                       (0x0100)
#define I2C_CR1_STOP                        (0x0200)
#define I2C_CR1_ACK                         (0x0400)
#define I2C_CR1_POS                         (0x0800)
#define I2C_CR1_PEC                         (0x1000)
#define I2C_CR1_ALERT                       (0x2000)
#define I2C_CR1_SWRST                       (0x8000)

/* I2C Control register 2 */
#define I2C_CR2_FREQ                        (0x003F)
#define I2C_CR2_FREQ_0                      (0x0001)
#define I2C_CR2_FREQ_1                      (0x0002)
#define I2C_CR2_FREQ_2                      (0x0004)
#define I2C_CR2_FREQ_3                      (0x0008)
#define I2C_CR2_FREQ_4                      (0x0010)
#define I2C_CR2_FREQ_5                      (0x0020)

#define I2C_CR2_ITERREN                     (0x0100)
#define I2C_CR2_ITEVTEN                     (0x0200)
#define I2C_CR2_ITBUFEN                     (0x0400)
#define I2C_CR2_DMAEN                       (0x0800)
#define I2C_CR2_LAST                        (0x1000)

/* Own address register 1 */
#define I2C_OAR1_ADD1_7                     (0x00FE)
#define I2C_OAR1_ADD8_9                     (0x0300)

#define I2C_OAR1_ADD0                       (0x0001)
#define I2C_OAR1_ADD1                       (0x0002)
#define I2C_OAR1_ADD2                       (0x0004)
#define I2C_OAR1_ADD3                       (0x0008)
#define I2C_OAR1_ADD4                       (0x0010)
#define I2C_OAR1_ADD5                       (0x0020)
#define I2C_OAR1_ADD6                       (0x0040)
#define I2C_OAR1_ADD7                       (0x0080)
#define I2C_OAR1_ADD8                       (0x0100)
#define I2C_OAR1_ADD9                       (0x0200)

#define I2C_OAR1_ADDMODE                    (0x8000)

/* own address register 2 */
#define I2C_OAR2_ENDUAL                     (0x01)
#define I2C_OAR2_ADD2                       (0xFE)

/* data register */
#define I2C_DR_DR                           (0xFF)

/* status register 1 */
#define I2C_SR1_SB                          (0x0001)
#define I2C_SR1_ADDR                        (0x0002)
#define I2C_SR1_BTF                         (0x0004)
#define I2C_SR1_ADD10                       (0x0008)
#define I2C_SR1_STOPF                       (0x0010)
#define I2C_SR1_RXNE                        (0x0040)
#define I2C_SR1_TXE                         (0x0080)
#define I2C_SR1_BERR                        (0x0100)
#define I2C_SR1_ARLO                        (0x0200)
#define I2C_SR1_AF                          (0x0400)
#define I2C_SR1_OVR                         (0x0800)
#define I2C_SR1_PECERR                      (0x1000)
#define I2C_SR1_TIMEOUT                     (0x4000)
#define I2C_SR1_SMBALERT                    (0x8000)

/* status register 2 */
#define I2C_SR2_MSL                         (0x0001)
#define I2C_SR2_BUSY                        (0x0002)
#define I2C_SR2_TRA                         (0x0004)
#define I2C_SR2_GENCALL                     (0x0010)
#define I2C_SR2_SMBDEFAULT                  (0x0020)
#define I2C_SR2_SMBHOST                     (0x0040)
#define I2C_SR2_DUALF                       (0x0080)
#define I2C_SR2_PEC                         (0xFF00)

/* clock control register */
#define I2C_CCR_CCR                         (0x0FFF)
#define I2C_CCR_DUTY                        (0x4000)
#define I2C_CCR_FS                          (0x8000)

/* TRISE register */
#define I2C_TRISE_TRISE                     (0x3F)


#endif /* STM32L1_I2C_H_ */
