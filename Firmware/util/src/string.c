/*
 * sstring.c
 *
 *  Created on: 22-12-2012
 *      Author: Tomek
 */

#include <stdint.h>
#include <util/size.h>

/* memory compare */
int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	const uint8_t *p1 = ptr1;
	const uint8_t *p2 = ptr2;

	/* memory compare */
	while (num > 0) {
		/* greater than */
		if (*p1 > *p2) {
			return 1;
		/* less than */
		} else if (*p1 < *p2) {
			return -1;
		}
		/* next byte */
		p1++; p2++; num--;
	}

	/* all equal */
	return 0;
}

/* memory area copy  */
void * memcpy(void *dst, const void *src, size_t size)
{
	/* pointers */
	uint8_t *d = dst;
	const uint8_t *s = src;

	/* copy memory */
	while (size-- > 0)
		*(d++) = *(s++);

	/* return destination */
	return dst;
}

/* memory area set */
void * memset(void *ptr, int value, size_t size)
{
	/* pointers */
	uint8_t *p = ptr;

	/* fill memory */
	while (size-- > 0)
		*(p++) = value;

	/* return pointer */
	return ptr;
}

/* return string length */
int strlen(const void *ptr)
{
	/* data pointer */
	const char *p = ptr;
	/* size */
	int size = 0;

	/* loop till end of string is found */
	while (*p++)
		size++;

	return size;
}
