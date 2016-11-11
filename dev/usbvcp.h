/*
 * usbvcp.h
 *
 *  Created on: 09-08-2015
 *      Author: Tomek
 */

#ifndef DEV_USBVCP_H_
#define DEV_USBVCP_H_

#include <util/size.h>
#include <sys/cb.h>

/* init hid */
int USBVCP_Init(void);
/* write data */
int USBVCP_Send(void *ptr, size_t size, cb_t cb);
/* read data */
int USBVCP_Recv(void *ptr, size_t size, cb_t cb);

#endif /* DEV_USBVCP_H_ */
