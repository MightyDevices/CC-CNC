/*
 * sr.h
 *
 *  Created on: Aug 28, 2016
 *      Author: Tomek
 */

#ifndef DEV_SR_H_
#define DEV_SR_H_

#include <stdint.h>

/* state bits */
#define SR_X_MD0							0x0100
#define SR_X_MD1							0x0200
#define SR_X_MD2							0x0400
#define SR_X_ENBL							0x0800
#define SR_Y_MD2							0x1000
#define SR_Y_MD1							0x2000
#define SR_Y_MD0							0x4000
#define SR_Y_ENBL							0x8000

#define SR_Z_MD2							0x0001
#define SR_Z_MD1							0x0002
#define SR_Z_MD0							0x0004
#define SR_Z_ENBL							0x0008
#define SR_A_MD2							0x0010
#define SR_A_MD1							0x0020
#define SR_A_MD0							0x0040
#define SR_A_ENBL							0x0080

/* shift register initialize */
int SR_Init(void);
/* set shift registeres output */
int SR_SetState(uint16_t state);

#endif /* DEV_SR_H_ */
