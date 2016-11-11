/*
 * debug.c
 *
 *  Created on: 25-05-2012
 *      Author: Tomek
 */

#include <dev/usart1.h>

#define DEBUG
#include <dev/debug.h>

/* debug string buffer */
char debug_buf[256];

/* callback used for async calls */
void Debug_USART1Callback(void *arg)
{
	/* simply release the lock */
	Sem_Release(&usart1_sem);
}

/* dump memory */
void Debug_DumpMem(void *ptr, size_t size)
{
/* debug is enabled? */
#ifdef DEBUG
	/* current index */
	size_t i;
	/* data pointer */
	uint8_t *p = (uint8_t *)ptr;

	/* for every byte */
	for (i = 0; i < size; i++) {
		/* start of line */
		if (i % 16 == 0)
			dprintf_l("0x%x: ", (unsigned int)i);
		/* byte and separator */
		dprintf_l("0x%x%c", p[i], i % 16 < 15 ? ' ' : '\n');
	}

	/* end of data */
	if (i % 16)
		dprintf_l("\n", 0);

/* #ifdef DEBUG */
#endif
}
