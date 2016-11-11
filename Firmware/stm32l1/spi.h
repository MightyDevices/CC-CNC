/*
 * spi.h
 *
 *  Created on: 31-07-2012
 *      Author: Tomek
 */

#ifndef STM32L1_SPI_H_
#define STM32L1_SPI_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define SPI1_BASE							0x40013000
#define SPI2_BASE							0x40003800
#define SPI3_BASE							0x40003C00

/* spis */
#define SPI1              					((spi_t *) SPI1_BASE)
#define SPI2              					((spi_t *) SPI2_BASE)
#define SPI3              					((spi_t *) SPI3_BASE)

/* spi registers */
typedef struct {
	reg16_t CR1;
	reg16_t RESERVED0;
	reg16_t CR2;
	reg16_t RESERVED1;
	reg16_t SR;
	reg16_t RESERVED2;
	reg16_t DR;
	reg16_t RESERVED3;
	reg16_t CRCPR;
	reg16_t RESERVED4;
	reg16_t RXCRCR;
	reg16_t RESERVED5;
	reg16_t TXCRCR;
	reg16_t RESERVED6;
	reg16_t I2SCFGR;
	reg16_t RESERVED7;
	reg16_t I2SPR;
	reg16_t RESERVED8;
} spi_t;

/* SPI control register 1 */
#define SPI_CR1_CPHA                        (0x0001)
#define SPI_CR1_CPOL                        (0x0002)
#define SPI_CR1_MSTR                        (0x0004)
#define SPI_CR1_BR                          (0x0038)
#define SPI_CR1_BR_0                        (0x0008)
#define SPI_CR1_BR_1                        (0x0010)
#define SPI_CR1_BR_2                        (0x0020)
#define SPI_CR1_SPE                         (0x0040)
#define SPI_CR1_LSBFIRST                    (0x0080)
#define SPI_CR1_SSI                         (0x0100)
#define SPI_CR1_SSM                         (0x0200)
#define SPI_CR1_RXONLY                      (0x0400)
#define SPI_CR1_DFF                         (0x0800)
#define SPI_CR1_CRCNEXT                     (0x1000)
#define SPI_CR1_CRCEN                       (0x2000)
#define SPI_CR1_BIDIOE                      (0x4000)
#define SPI_CR1_BIDIMODE                    (0x8000)

/* SPI control register 2 */
#define SPI_CR2_RXDMAEN                     (0x01)
#define SPI_CR2_TXDMAEN                     (0x02)
#define SPI_CR2_SSOE                        (0x04)
#define SPI_CR2_FRF                         (0x08)
#define SPI_CR2_ERRIE                       (0x20)
#define SPI_CR2_RXNEIE                      (0x40)
#define SPI_CR2_TXEIE                       (0x80)

/* SPI status register */
#define SPI_SR_RXNE                         (0x01)
#define SPI_SR_TXE                          (0x02)
#define SPI_SR_CHSIDE                       (0x04)
#define SPI_SR_UDR                          (0x08)
#define SPI_SR_CRCERR                       (0x10)
#define SPI_SR_MODF                         (0x20)
#define SPI_SR_OVR                          (0x40)
#define SPI_SR_BSY                          (0x80)

/* SPI data register */
#define SPI_DR_DR                           (0xFFFF)

/* SPI CRC polynomial register */
#define SPI_CRCPR_CRCPOLY                   (0xFFFF)

/* SPI RX CRC register */
#define SPI_RXCRCR_RXCRC                    (0xFFFF)

/* SPI TX CRC register */
#define SPI_TXCRCR_TXCRC                    (0xFFFF)

/* SPI_I2S configuration register */
#define SPI_I2SCFGR_CHLEN                   (0x0001)
#define SPI_I2SCFGR_DATLEN                  (0x0006)
#define SPI_I2SCFGR_DATLEN_0                (0x0002)
#define SPI_I2SCFGR_DATLEN_1                (0x0004)
#define SPI_I2SCFGR_CKPOL                   (0x0008)
#define SPI_I2SCFGR_I2SSTD                  (0x0030)
#define SPI_I2SCFGR_I2SSTD_0                (0x0010)
#define SPI_I2SCFGR_I2SSTD_1                (0x0020)
#define SPI_I2SCFGR_PCMSYNC                 (0x0080)
#define SPI_I2SCFGR_I2SCFG                  (0x0300)
#define SPI_I2SCFGR_I2SCFG_0                (0x0100)
#define SPI_I2SCFGR_I2SCFG_1                (0x0200)
#define SPI_I2SCFGR_I2SE                    (0x0400)
#define SPI_I2SCFGR_I2SMOD                  (0x0800)

/* SPI_I2S prescaler register */
#define SPI_I2SPR_I2SDIV                    (0x00FF)
#define SPI_I2SPR_ODD                       (0x0100)
#define SPI_I2SPR_MCKOE                     (0x0200)


#endif /* STM32L1_SPI_H_ */
