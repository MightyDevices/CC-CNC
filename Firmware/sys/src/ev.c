/*
 * ev.c
 *
 *  Created on: 07-06-2013
 *      Author: Tomek
 */

#include <sys/ev.h>
#include <sys/err.h>
#include <util/string.h>

/* send notification */
void Ev_Notify(ev_t *ev, void *arg)
{
	cb_t *cb = ev->cb;
	/* max number of callbacks */
	int i = EV_MAXCALLS;

	/* call callbacks */
	while (i--) {
		/* callback present? */
		if (*cb)
			(*cb)(arg);
		/* next callback */
		cb++;
	}
}

/* register callback */
int Ev_RegisterCallback(ev_t *ev, cb_t callback)
{
	/* max number of callbacks */
	int i = EV_MAXCALLS;
	/* callback pointer, free callback */
	cb_t *cb = ev->cb, *fcb = 0;

	/* look for slot */
	while (i--) {
		/* found free slot */
		if (*cb == 0 && fcb == 0) {
			fcb = cb;
		/* already registered? */
		} else if (*cb == callback) {
			return EFATAL;
		}
		/* next slot */
		cb++;
	}

	/* found free slot? */
	if (fcb)
		*fcb = callback;

	/* report status */
	return fcb != 0 ? EOK : EFATAL;
}

/* unregister callback */
int Ev_UnregisterCallback(ev_t *ev, cb_t callback)
{
	/* max number of callbacks */
	int i = EV_MAXCALLS;
	/* callback pointer, free callback */
	cb_t *cb = ev->cb;

	/* look for callback */
	while (i--) {
		/* found callback? */
		if (*cb == callback) {
			/* reset entry */
			*cb = 0;
			/* not much to do */
			return EOK;
		}
	}

	/* report status */
	return EFATAL;
}
