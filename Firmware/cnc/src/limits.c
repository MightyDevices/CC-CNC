/*
 * limits.c
 *
 *  Created on: Oct 23, 2016
 *      Author: Tomek
 */

#include <dev/mcp23008.h>

/* get limit switches status */
int Limits_GetStatus(void)
{
	/* return current pin state as seen by gpio expander */
	return MCP23008_GetStatus();
}
