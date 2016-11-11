/*
 * watchdog.h
 *
 *  Created on: 13-02-2013
 *      Author: Tomek
 */

#ifndef DEV_WATCHDOG_H_
#define DEV_WATCHDOG_H_

/* watchdog interrupt routine */
void Watchdog_WWDGIsr(void);

/* enable hardware watchdog */
int Watchdog_Init(void);
/* kick the dog! */
void Watchdog_Kick(void);

#endif /* WATCHDOG_H_ */
