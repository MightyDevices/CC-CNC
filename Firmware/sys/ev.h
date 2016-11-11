/*
 * ev.h
 *
 *  Created on: 07-06-2013
 *      Author: Tomek
 */

#ifndef SYS_EV_H_
#define SYS_EV_H_

#include <stdint.h>
#include <sys/cb.h>

/* maximal number of events */
#define EV_MAXCALLS						5

/* system event */
typedef struct {
	/* number of callbacks, event lock */
	uint8_t cb_num, lock;
	/* callback */
	cb_t cb[EV_MAXCALLS];
} ev_t;

/* initialize event */
void Ev_Init(ev_t *ev);
/* send notification */
void Ev_Notify(ev_t *ev, void *arg);
/* register callback */
int Ev_RegisterCallback(ev_t *ev, cb_t callback);
/* unregister callback */
int Ev_UnregisterCallback(ev_t *ev, cb_t callback);

#endif /* EV_H_ */
