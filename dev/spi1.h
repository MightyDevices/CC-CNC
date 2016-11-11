/*
 * spi1.h
 *
 *  Created on: Aug 28, 2016
 *      Author: Tomek
 */

#ifndef DEV_SPI1_H_
#define DEV_SPI1_H_

#include <stdint.h>

/* initialize spi */
int SPI1_Init(void);
/* send 16-bit word over spi1 */
int SPI1_Send16b(uint16_t word);

#endif /* DEV_SPI1_H_ */
