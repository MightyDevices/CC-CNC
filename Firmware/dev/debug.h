/*
 * debug.h
 *
 *  Created on: 24-05-2012
 *      Author: Tomek
 */

#ifndef DEV_DEBUG_H_
#define DEV_DEBUG_H_

#include <dev/usart1.h>
#include <dev/systime.h>
#include <util/stdio.h>
#include <util/string.h>
#include <util/size.h>
#include <sys/err.h>
#include <sys/cb.h>

/* disables debug globally */
//#undef DEBUG

/* debug enabled? */
#ifdef DEBUG

/* debug string buffer */
extern char debug_buf[];

/* main debug routine for 'puts' like calls */
#define dputs(fmt)															\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {                      												\
		/* lock the resource */												\
		Sem_Lock(&usart1_sem);												\
		/* produce string */												\
		int __l = sprintf(debug_buf, "["__FILE__":%d]" fmt, __LINE__);		\
		/* send string */													\
		USART1_Send(debug_buf, __l, CB_NULL);								\
		/* unlock */														\
		Sem_Release(&usart1_sem);											\
	} while (0)

/* main debug routine */
#define dprintf(fmt, ...)													\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {                      												\
		/* lock the resource */												\
		Sem_Lock(&usart1_sem);												\
		/* produce string */												\
		int __l = sprintf(debug_buf, "["__FILE__":%d]" fmt, __LINE__, 		\
						## __VA_ARGS__);									\
		/* send string */													\
		USART1_Send(debug_buf, __l, CB_NULL);								\
		/* unlock */														\
		Sem_Release(&usart1_sem);											\
	} while (0)

/* debug without line number  */
#define dprintf_l(fmt, ...)													\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {                      												\
		/* lock the resource */												\
		Sem_Lock(&usart1_sem);												\
		/* produce string */												\
		int __l = sprintf(debug_buf, fmt, ## __VA_ARGS__);					\
		/* send string */													\
		USART1_Send(debug_buf, __l, CB_NULL);								\
		/* unlock */														\
		Sem_Release(&usart1_sem);											\
	} while (0)

/* non blocking debug routine (use in interrupts) */
#define dprintf_i(fmt, ...)													\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {                      												\
		SysTime_Delay(5);													\
		/* try lock the resource */											\
		if (Sem_TryLock(&usart1_sem) != EOK)								\
			break;															\
		/* produce string */												\
		int __l = sprintf(debug_buf, "["__FILE__":%d]" fmt, __LINE__, 		\
						## __VA_ARGS__);									\
		/* send string */													\
		USART1_Send(debug_buf, __l, Debug_USART1Callback);					\
	} while (0)

/* critical debug routine (use for stack printing in critical faults) */
#define dprintf_c(fmt, ...)													\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {																	\
		/* produce string */												\
		int __l = sprintf(debug_buf, "["__FILE__":%d]" fmt, __LINE__, 		\
						## __VA_ARGS__);									\
		/* send string */													\
		USART1_CriticalSend(debug_buf, __l);								\
	} while (0)

/* dump memory buffer */
#define dmem(ptr, size)														\
	/* call function */														\
	Debug_DumpMem(ptr, size)

/* debug disabled */
#else

/* dputs */
#define dputs(str)															\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {																	\
	} while (0)

/* main debug routine */
#define dprintf(fmt, ...)													\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {																	\
	} while (0)

/* main debug routine */
#define dprintf_i(fmt, ...)													\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {																	\
	} while (0)


/* critical debug routine */
#define dprintf_c(fmt, ...)													\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {																	\
	} while (0)

/* dump memory buffer */
#define dmem(ptr, size)														\
	/* encapsulated in a loop, to make it compiler-proof :) */				\
	do {																	\
	} while (0)


#endif

void Debug_USART1Callback(void *arg);
/* dump memory */
void Debug_DumpMem(void *ptr, size_t size);

#endif /* DEBUG_H_ */
