/*
 * string.h
 *
 *  Created on: 22-12-2012
 *      Author: Tomek
 */

#ifndef UTIL_STRING_H_
#define UTIL_STRING_H_

#include <util/size.h>

/* memory compare */
int memcmp(const void *ptr1, const void *ptr2, size_t num);
/* memcpy */
void * memcpy(void *dst, const void *src, size_t num);
/* memset */
void * memset(void *ptr, int value, size_t num);
/* return string length */
int strlen(const void *ptr);

#endif /* SSTRING_H_ */
