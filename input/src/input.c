/*
 * input.c
 *
 *  Created on: Oct 16, 2016
 *      Author: Tomek
 */

#include <cnc/gcode.h>
#include <sys/err.h>
#include <dev/usbvcp.h>
#include <util/size.h>
#include <util/string.h>

#define DEBUG
#include <dev/debug.h>

/* input line */
static char line[256];
/* input buffer */
static uint8_t buf[32];
/* buffer size, line length */
static uint32_t line_size, buf_size;

/* receive callback */
static void Input_VCPCallback(void *arg)
{
	/* transfer size */
	buf_size = *((size_t *)arg);
	/* empty reception? simply restart transfer */
	if (!buf_size)
		USBVCP_Recv(buf, sizeof(buf), Input_VCPCallback);
}

/* initialize input routines */
int Input_Init(void)
{
	/* start reception */
	USBVCP_Recv(buf, sizeof(buf), Input_VCPCallback);
	/* report status */
	return EOK;
}

/* process data */
void Input_Poll(void)
{
	/* nothing to process */
	if (buf_size == 0)
		return;

	/* process char by char */
	for (uint8_t *p = buf; buf_size; buf_size--, p++) {
		/* normal char? */
		if (*p != '\n') {
			/* prevent overflows */
			if (line_size < sizeof(line))
				line[line_size++] = *p;
		/* newline? */
		} else {
			/* terminate line */
			line[line_size] = '\0';
			/* execute line if there was no overflow */
			if (line_size < sizeof(line)) {
				int rc = GCode_Input(line);
				if (rc != 0)
					dprintf("error: %d\n", rc);
			}
			/* reset line size */
			line_size = 0;
		}
	}

	/* restart transfer */
	USBVCP_Recv(buf, sizeof(buf), Input_VCPCallback);
}
