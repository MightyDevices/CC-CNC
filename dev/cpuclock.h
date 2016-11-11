/*
 * cpuclock.h
 *
 *  Created on: 16-09-2015
 *      Author: Tomek
 */

#ifndef DEV_CPUCLOCK_H_
#define DEV_CPUCLOCK_H_

#include <sys/ev.h>

/* clock frequencies */
#define CPUCLOCK_1M048						0x04
#define CPUCLOCK_2M096						0x05
#define CPUCLOCK_4M192						0x06
#define CPUCLOCK_16M						0x07
#define CPUCLOCK_32M						0x08

/* initialize frequency scaling module */
int CpuClock_Init(void);
/* get current clock frequency */
int CpuClock_GetFreq(void);
/* set frequency */
void CpuClock_SetFreq(int freq);


#endif /* DEV_CPUCLOCK_H_ */
