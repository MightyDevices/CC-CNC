/*
 * time.h
 *
 *  Created on: 11-06-2013
 *      Author: Tomek
 */

#ifndef SYS_TIME_H_
#define SYS_TIME_H_

#include <stdint.h>
#include <stm32l1/timer.h>

/* default time type */
typedef uint32_t time_t;
/* default time difference type */
typedef int32_t dtime_t;

/* get time macro */
#define time(x)								(time_t)(TIM5->CNT)
/* get time difference */
#define dtime(a, b)							(dtime_t)((a) -(b))

#endif /* SYS_TIME_H_ */

