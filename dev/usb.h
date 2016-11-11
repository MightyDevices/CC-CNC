/*
 * usb.h
 *
 *  Created on: 15-02-2014
 *      Author: Tomek
 */

#ifndef DEV_USB_H_
#define DEV_USB_H_

#include <stdint.h>
#include <util/size.h>
#include <sys/ev.h>

/* enpoint numbers */
/* enpoint 0 */
#define USB_EP0								0x00
/* enpoint 1 */
#define USB_EP1								0x01
/* enpoint 2 */
#define USB_EP2								0x02
/* enpoint 3 */
#define USB_EP3								0x03
/* enpoint 4 */
#define USB_EP4								0x04
/* enpoint 5 */
#define USB_EP5								0x05
/* enpoint 6 */
#define USB_EP6								0x06
/* enpoint 7 */
#define USB_EP7								0x07

/* Mask to get recipient */
#define USB_SETUP_REQTYPE_RECIPIENT         0x1F
/* Recipient device */
#define USB_SETUP_REQTYPE_RECIPIENT_DEVICE	0x00
/* Recipient interface */
#define USB_SETUP_REQTYPE_RECIPIENT_IFACE	0x01
/* Recipient endpoint */
#define USB_SETUP_REQTYPE_RECIPIENT_EP		0x02
/* Other recipient */
#define USB_SETUP_REQTYPE_RECIPIENT_OTHER	0x03

/* type mask */
#define USB_SETUP_REQTYPE_TYPE         		0x60
/* standard request */
#define USB_SETUP_REQTYPE_TYPE_STANDARD		0x00
/* class specific request */
#define USB_SETUP_REQTYPE_TYPE_CLASS		0x20
/* vendor specific request */
#define USB_SETUP_REQTYPE_TYPE_VENDOR		0x40
/* reserved */
#define USB_SETUP_REQTYPE_TYPE_RESERVED		0x60

/* direction bit */
#define USB_SETUP_REQTYPE_DIR         		0x80
/* from host to device */
#define USB_SETUP_REQTYPE_DIR_OUT			0x00
/* from device to host */
#define USB_SETUP_REQTYPE_DIR_IN			0x80

/* request field  */
/* get status */
#define USB_SETUP_REQ_GET_STATUS			0x00
/* clear feature */
#define USB_SETUP_REQ_CLEAR_FEATURE			0x01
/* set feature */
#define USB_SETUP_REQ_SET_FEATURE			0x03
/* set device address */
#define USB_SETUP_REQ_SET_ADDRESS			0x05
/* get descriptor */
#define USB_SETUP_REQ_GET_DESCRIPTOR		0x06
/* set descriptor */
#define USB_SETUP_REQ_SET_DESCRIPTOR		0x07
/* get status */
#define USB_SETUP_REQ_GET_CONFIGURATION		0x08
/* set configuration */
#define USB_SETUP_REQ_SET_CONFIGURATION		0x09
/* get interface */
#define USB_SETUP_REQ_GET_INTERFACE			0x0A
/* set interface */
#define USB_SETUP_REQ_SET_INTERFACE			0x0B
/* synch frame */
#define USB_SETUP_REQ_SYNCH_FRAME			0x0C

/* descriptor types */
/* device descriptor */
#define USB_SETUP_DESCTYPE_DEVICE			0x01
/* configuration descriptor */
#define USB_SETUP_DESCTYPE_CONFIGURATION	0x02
/* string descriptor */
#define USB_SETUP_DESCTYPE_STRING			0x03
/* interface descriptor */
#define USB_SETUP_DESCTYPE_INTERFACE		0x04
/* enpoint descritpro */
#define USB_SETUP_DESCTYPE_ENDPOINT			0x05

/* standard features */
/* halt endpoint */
#define USB_SETUP_FEATURE_ENDPOINT_HALT		0x00
/* device remote wakeup */
#define USB_SETUP_FEATURE_DEV_REMOTE_WKUP	0x01
/* test mode */
#define USB_SETUP_FEATURE_TEST_MODE			0x02

/* status bits */
/* self powered device */
#define USB_SETUP_STATUS_DEV_SELF_POW		0x01
/* remote wakeup */
#define USB_SETUP_STATUS_DEV_REMOTE_WKUP	0x02
/* endpoint halted */
#define USB_SETUP_STATUS_EP_HALT			0x01

/* device states */
/* defaul (after-reset) state */
#define USB_DEV_DEFAULT						0x00
/* addressing state */
#define USB_DEV_ADDRESS						0x01
/* device is fully configured and functional */
#define USB_DEV_CONFIGURED					0x02

/* endpoint callback flags */
/* receive */
#define USB_EP_RX							0x01
/* transmit */
#define USB_EP_TX							0x02

/* setup frame */
typedef struct {
	/* bmRequestType */
	uint8_t	request_type;
	/* bRequest */
	uint8_t	request;
	/* wValue */
	uint16_t value;
	/* wIndex */
	uint16_t index;
	/* wLength */
	uint16_t length;
} __attribute__((packed)) usb_setup_t;

/* callbacks */
typedef struct {
	/* reset callback */
	void (*reset) (void);
	/* device request with data stage */
	int (*request) (usb_setup_t *, void **, size_t *);
	/* status callback */
	void (*status) (void);
	/* endpoint data callbacks */
	int (*endpoint[8]) (int type);
} usb_callback_t;

/* class/interface specific request */
typedef struct {
	/* setup frame that causet the request */
	usb_setup_t *setup;
	/* request status */
	int status;
	/* data address returned */
	void *ptr;
	/* size of data */
	size_t size;
} usb_req_evarg_t;

/* endpoint data transfer event */
typedef struct {
	/* endpoint number, transfer type */
	int ep_num, type;
} usb_ep_evarg_t;

/* system events: reset event, request event, endpoint event */
extern ev_t usb_rst_ev, usb_req_ev, usb_ep_ev;

/* interrupt service routine */
void USB_LPIrq(void);
/* high priority service routine */
void USB_HPIrq(void);

/* init */
int USB_Init(void);
/* deinit usb */
int USB_Deinit(void);
/* perform usb reset */
void USB_Reset(void);
/* set usb callbacks */
void USB_SetCallbacks(usb_callback_t *cb);

/* set endpoint address and type */
void USB_SetEndpointTypeAddress(int ep_num, int type, int address);

/* set endpoint receive buffer offset (from the start of btable) & size */
void USB_SetEndpointRxBuffer(int ep_num, int offset, size_t size);
/* set endpoint transmit buffer offset & size*/
void USB_SetEndpointTxBuffer(int ep_num, int offset, size_t size);

/* set endpoint status (used for marking buffer as ready-to-receive) */
void USB_SetEndpointRxStatus(int ep_num, uint16_t status);
/* set endpoint status (used for marking buffer as ready-to-send) */
void USB_SetEndpointTxStatus(int ep_num, uint16_t status);
/* read from endpoint rx buffer, ptr must be uint16_t aligned, max size must be
 * even */

int USB_ReadEndpointBuffer(int ep_num, void *ptr, size_t size);
/* write to endpoint tx buffer */
int USB_WriteEndpointBuffer(int ep_num, void *ptr, size_t size);

#endif /* USB_H_ */
