/*
 * pvd.h
 *
 *  Created on: 13-04-2014
 *      Author: Tomek
 */

#ifndef DEV_POWER_H_
#define DEV_POWER_H_

#include <stdint.h>

/* pvd levels */
#define POWER_LEVEL_1V9                  	(0x0000)
#define POWER_LEVEL_2V1                  	(0x0020)
#define POWER_LEVEL_2V3                  	(0x0040)
#define POWER_LEVEL_2V5                  	(0x0060)
#define POWER_LEVEL_2V7                  	(0x0080)
#define POWER_LEVEL_2V9                  	(0x00A0)
#define POWER_LEVEL_3V1                  	(0x00C0)

/* exti lines */
#define POWER_EXTI_P0						(1 << 0)
#define POWER_EXTI_P1						(1 << 1)
#define POWER_EXTI_P2						(1 << 2)
#define POWER_EXTI_P3						(1 << 3)
#define POWER_EXTI_P4						(1 << 4)
#define POWER_EXTI_P5						(1 << 5)
#define POWER_EXTI_P6						(1 << 6)
#define POWER_EXTI_P7						(1 << 7)
#define POWER_EXTI_P8						(1 << 8)
#define POWER_EXTI_P9						(1 << 9)
#define POWER_EXTI_P10						(1 << 10)
#define POWER_EXTI_P11						(1 << 11)
#define POWER_EXTI_P12						(1 << 12)
#define POWER_EXTI_P13						(1 << 13)
#define POWER_EXTI_P14						(1 << 14)
#define POWER_EXTI_P15						(1 << 15)
#define POWER_EXTI_PVD						(1 << 16)
#define POWER_EXTI_RTC						(1 << 20)

/* power level detector init */
int Power_Init(void);
/* enter sleep mode */
void Power_SleepMode(void);
/* enter stop mode, exit on exti event */
void Power_StopMode(void);

#endif /* PVD_H_ */
