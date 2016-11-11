/*
 * mcp23008.h
 *
 *  Created on: Oct 22, 2016
 *      Author: Tomek
 */

#ifndef DEV_MCP23008_H_
#define DEV_MCP23008_H_

/* interrupt service routine */
void MCP23008_Exti13Isr(void);
/* initialize gpio expander */
int MCP23008_Init(void);
/* return current pin status */
int MCP23008_GetStatus(void);

#endif /* DEV_MCP23008_H_ */
