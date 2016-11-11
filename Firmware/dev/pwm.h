/*
 * pwm.h
 *
 *  Created on: Oct 22, 2016
 *      Author: Tomek
 */

#ifndef DEV_PWM_H_
#define DEV_PWM_H_

/* initialize pwm */
int Pwm_Init(void);
/* sets duty cycle */
int Pwm_SetDutyCycle(int dc);

#endif /* DEV_PWM_H_ */
