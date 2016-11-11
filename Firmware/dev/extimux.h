/*
 * extimux.h
 *
 *  Created on: 31-08-2015
 *      Author: Tomek
 */

#ifndef DEV_EXTIMUX_H_
#define DEV_EXTIMUX_H_

/* exti interrupt mux for lines 5 to 9 */
void ExtiMux_Exti5_9Isr(void);
/* exti interrupt mux for lines 10 to 15 */
void ExtiMux_Exti10_15Isr(void);
/* initialize exti mux */
int ExtiMux_Init(void);

#endif /* DEV_EXTIMUX_H_ */
