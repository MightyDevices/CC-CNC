/*
 * i2c1.h
 *
 *  Created on: 02-07-2015
 *      Author: Tomek
 */

#ifndef DEV_I2C1_H_
#define DEV_I2C1_H_

#include <sys/cb.h>
#include <sys/sem.h>
#include <util/size.h>
#include <stdint.h>

/* i2c operations */
/* write to register */
#define I2C1_WRITE							0
/* read register */
#define I2C1_READ							1

/* transfer lock */
extern sem_t i2c1_sem;

/* callback argument */
typedef struct {
	/* error flags */
	int error;
} i2c1_cbarg_t;

/* event interrupt */
void I2C1_I2C1EvIsr(void);
/* error interrupt routine */
void I2C1_I2C1ErrIsr(void);
/* tx dma routine */
void I2C1_DMA1Ch6Isr(void);
/* rx dma routine */
void I2C1_DMA1Ch7Isr(void);

/* initialize i2c1 */
int I2C1_Init(void);
/* i2c transfer */
int I2C1_Transfer(int oper_type, uint8_t addr, uint8_t sub_addr, void *ptr,
		size_t size, cb_t cb);

#endif /* I2C1_H_ */
