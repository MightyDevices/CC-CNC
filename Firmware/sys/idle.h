/*
 * idle.h
 *
 *  Created on: 03-07-2015
 *      Author: Tomek
 */

#ifndef SYS_IDLE_H_
#define SYS_IDLE_H_

#include <stdint.h>
#include <sys/time.h>

/* idle modes */
#define IDLE_CORE						0x02
#define IDLE_PERIPH						0x01
#define IDLE_STOP						0x00

/* initialize idle mode monitor */
int Idle_Init(void);
/* polling in idle mode */
void Idle_Poll(void);
/* set idle mode */
void Idle_SetMode(int mode, uint32_t channel);
/* release idle mode */
void Idle_ReleaseMode(int mode, uint32_t channel);
/* wakes up for given timestamp */
void Idle_Wakeup(time_t tow);

#endif /* IDLE_H_ */
