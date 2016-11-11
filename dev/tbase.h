/*
 * tbase.h
 *
 *  Created on: 01-01-2016
 *      Author: Tomek
 */

#ifndef DEV_TBASE_H_
#define DEV_TBASE_H_

#include <sys/ev.h>

/* time base tick event */
extern ev_t tbase_ev;

/* interrupt service routine */
void TBase_Tim2Isr(void);
/* initialize tests */
int TBase_Init(void);
/* set prescaler */
int TBase_SetPrescaler(int value);

#endif /* DEV_TBASE_H_ */
