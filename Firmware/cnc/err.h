/*
 * err.h
 *
 *  Created on: Oct 15, 2016
 *      Author: Tomek
 */

#ifndef ERR_H_
#define ERR_H_

/* no error */
#define ERR_OK								0
/* general fatal error */
#define ERR_FATAL							-1

/* word related errors */
/* no axis words */
#define ERR_WORD_MISSING					-10
/* word value not allowed */
#define ERR_WORD_VALUE						-11
/* too many words specified */
#define ERR_WORD_EXCESS						-12


#endif /* ERR_H_ */
