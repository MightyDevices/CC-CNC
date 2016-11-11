/*
 * sem.h
 *
 *  Created on: 03-07-2015
 *      Author: Tomek
 */

#ifndef SYS_SEM_H_
#define SYS_SEM_H_

#include <stdint.h>

/* semaphore type */
typedef volatile uint32_t sem_t;

/* try to lock on semaphore */
int Sem_TryLock(sem_t *s);
/* lock */
int Sem_Lock(sem_t *s);
/* release semaphore */
void Sem_Release(sem_t *s);

#endif /* SEM_H_ */
