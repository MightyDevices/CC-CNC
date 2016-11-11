/*
 * usart.h
 *
 *  Created on: 13-06-2012
 *      Author: Tomek
 */

#ifndef STM32L1_USART_H_
#define STM32L1_USART_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define USART1_BASE							0x40013800
#define USART2_BASE							0x40004400
#define USART3_BASE							0x40004800
#define USART4_BASE							0x40004c00
#define USART5_BASE							0x40005000

/* usart */
#define USART1              				((usart_t *) USART1_BASE)
#define USART2              				((usart_t *) USART2_BASE)
#define USART3              				((usart_t *) USART3_BASE)
#define USART4              				((usart_t *) USART4_BASE)
#define USART5             	 				((usart_t *) USART5_BASE)


/* usart registers */
typedef struct {
	reg16_t SR;
	reg16_t RESERVED0;
	reg16_t DR;
	reg16_t RESERVED1;
	reg16_t BRR;
	reg16_t RESERVED2;
	reg16_t CR1;
	reg16_t RESERVED3;
	reg16_t CR2;
	reg16_t RESERVED4;
	reg16_t CR3;
	reg16_t RESERVED5;
	reg16_t GTPR;
	reg16_t RESERVED6;
} __attribute__((packed)) usart_t;

/* Status register */
#define USART_SR_PE                         (0x0001)
#define USART_SR_FE                         (0x0002)
#define USART_SR_NE                         (0x0004)
#define USART_SR_ORE                        (0x0008)
#define USART_SR_IDLE                       (0x0010)
#define USART_SR_RXNE                       (0x0020)
#define USART_SR_TC                         (0x0040)
#define USART_SR_TXE                        (0x0080)
#define USART_SR_LBD                        (0x0100)
#define USART_SR_CTS                        (0x0200)

/* Data register */
#define USART_DR_DR                         (0x01FF)

/* Baud rate register */
#define USART_BRR_DIV_FRACTION              (0x000F)
#define USART_BRR_DIV_MANTISSA              (0xFFF0)

/* Control register 1 */
#define USART_CR1_SBK                       (0x0001)
#define USART_CR1_RWU                       (0x0002)
#define USART_CR1_RE                        (0x0004)
#define USART_CR1_TE                        (0x0008)
#define USART_CR1_IDLEIE                    (0x0010)
#define USART_CR1_RXNEIE                    (0x0020)
#define USART_CR1_TCIE                      (0x0040)
#define USART_CR1_TXEIE                     (0x0080)
#define USART_CR1_PEIE                      (0x0100)
#define USART_CR1_PS                        (0x0200)
#define USART_CR1_PCE                       (0x0400)
#define USART_CR1_WAKE                      (0x0800)
#define USART_CR1_M                         (0x1000)
#define USART_CR1_UE                        (0x2000)
#define USART_CR1_OVER8                     (0x8000)

/* Control register 2 */
#define USART_CR2_ADD                       (0x000F)
#define USART_CR2_LBDL                      (0x0020)
#define USART_CR2_LBDIE                     (0x0040)
#define USART_CR2_LBCL                      (0x0100)
#define USART_CR2_CPHA                      (0x0200)
#define USART_CR2_CPOL                      (0x0400)
#define USART_CR2_CLKEN                     (0x0800)
#define USART_CR2_STOP                      (0x3000)
#define USART_CR2_STOP_0                    (0x1000)
#define USART_CR2_STOP_1                    (0x2000)
#define USART_CR2_LINEN                     (0x4000)

/* Control register 3 */
#define USART_CR3_EIE                       (0x0001)
#define USART_CR3_IREN                      (0x0002)
#define USART_CR3_IRLP                      (0x0004)
#define USART_CR3_HDSEL                     (0x0008)
#define USART_CR3_NACK                      (0x0010)
#define USART_CR3_SCEN                      (0x0020)
#define USART_CR3_DMAR                      (0x0040)
#define USART_CR3_DMAT                      (0x0080)
#define USART_CR3_RTSE                      (0x0100)
#define USART_CR3_CTSE                      (0x0200)
#define USART_CR3_CTSIE                     (0x0400)
#define USART_CR3_ONEBIT                    (0x0800)

/* Guard time and prescaler register */
#define USART_GTPR_PSC                      (0x00FF)
#define USART_GTPR_PSC_0                    (0x0001)
#define USART_GTPR_PSC_1                    (0x0002)
#define USART_GTPR_PSC_2                    (0x0004)
#define USART_GTPR_PSC_3                    (0x0008)
#define USART_GTPR_PSC_4                    (0x0010)
#define USART_GTPR_PSC_5                    (0x0020)
#define USART_GTPR_PSC_6                    (0x0040)
#define USART_GTPR_PSC_7                    (0x0080)
#define USART_GTPR_GT                       (0xFF00)

#endif /* STM32L1_USART_H_ */
