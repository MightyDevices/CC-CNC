/*
 * led.h
 *
 *  Created on: 28-02-2016
 *      Author: Tomek
 */

#ifndef DEV_LED_H_
#define DEV_LED_H_


/* busy led timer */
void Led_Tim10Isr(void);
/* err led timer */
void Led_Tim11Isr(void);

/* initialize leds */
int Led_Init(void);
/* configure led blinking fro busy led */
void Led_BsyBlink(int on, int period);
/* configure led blinking for err led */
void Led_ErrBlink(int on, int period);
void Led_ErrToggle(void);


#endif /* DEV_LED_H_ */
