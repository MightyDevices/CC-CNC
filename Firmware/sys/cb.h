/*
 * cb.h
 *
 *  Created on: 21-06-2015
 *      Author: Tomek
 */

#ifndef SYS_CB_H_
#define SYS_CB_H_

/* callback */
typedef void (*volatile cb_t) (void *);

/* no callback */
#define CB_NULL								(cb_t)(0)
/* empty callback */
#define CB_NONE								(cb_t)(-1)


#endif /* CB_H_ */
