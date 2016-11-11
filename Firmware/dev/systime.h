/*
 * systime.h
 *
 *  Created on: 11-06-2013
 *      Author: Tomek
 */

#ifndef DEV_SYSTIME_H_
#define DEV_SYSTIME_H_

#include <stdint.h>

/* system timer interrupt routine */
void SysTime_SysTickExc(void);

/* initialize system tick timer */
int SysTime_Init(void);
/* deinitialize system tick timer */
int SysTime_Deinit(void);
/* delay in ms */
void SysTime_Delay(uint32_t ms);
/* set current time */
void SysTime_SetTime(uint32_t t);

#endif /* SYSTICK_H_ */
