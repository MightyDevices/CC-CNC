/*
 * mcp4728.h
 *
 *  Created on: Aug 28, 2016
 *      Author: Tomek
 */

#ifndef DEV_MCP4728_H_
#define DEV_MCP4728_H_

#include <stdint.h>
#include <sys/cb.h>

/* initialize quad output dac */
int MCP4728_Init(void);
/* set four channel values */
void MCP4728_SetChannels(uint16_t ch[4], cb_t cb);

#endif /* DEV_MCP4728_H_ */
