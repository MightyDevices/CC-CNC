/*
 * usbvcp.c
 *
 *  Created on: 08-08-2015
 *      Author: Tomek
 */


#include <dev/usb.h>
#include <dev/usbvcp.h>
#include <stm32l1/usb.h>
#include <sys/err.h>
#include <util/minmax.h>

#define DEBUG
#include <dev/debug.h>

/* requests */
/* set asynchronous line character formatting */
#define USB_VCP_REQ_SET_LINE_CODING				0x20
/* get asynchronous line character formatting */
#define USB_VCP_REQ_GET_LINE_CODING				0x21
/* This request generates RS-232/V.24 style control signals. */
#define USB_VCP_SET_CONTROL_LINE_STATE			0x22

/* line encoding */
typedef struct {
	/* current baudrate */
	uint32_t bauds;
	/* stop bits */
	uint8_t stop_bits;
	/* parity type */
	uint8_t parity_type;
	/* data bits */
	uint8_t data_bits;
} __attribute__((packed)) le_t;
/* current line encoding: 115200bps, 1 stop bit, no parity, 8 bits */
static le_t le = {115200, 1, 0, 8};

/* callbacks */
static cb_t tx_cb, rx_cb;
/* buffer pointers */
static uint8_t *tx_ptr, *rx_ptr;
/* buffer size */
static size_t tx_size, rx_size;

/* semaphores */
sem_t vcptx_sem, vcprx_sem;

/* reset callback */
static void USBVCP_ResetCallback(void *arg)
{
	/* endpoint 1 - interrupt endpoint */
	/* set endpoint type and address */
	USB_SetEndpointTypeAddress(USB_EP1, USB_EP_TYPE_INT, 0x01);
	/* set tx address, just after rx buffer, do not set data count */
	USB_SetEndpointTxBuffer(USB_EP1, 0x60, 0x00);
	/* tx status is not ready  */
	USB_SetEndpointTxStatus(USB_EP1, USB_STAT_TX_NAK);
	/* receive status is ready */
	USB_SetEndpointRxStatus(USB_EP1, USB_STAT_RX_DISABLED);

	/* endpoint 2 - data in */
	/* set endpoint type */
	USB_SetEndpointTypeAddress(USB_EP2, USB_EP_TYPE_BULK, 0x02);
	/* set tx address, just after rx buffer, do not set data count */
	USB_SetEndpointTxBuffer(USB_EP2, 0x68, 0x00);
	/* tx status is not ready  */
	USB_SetEndpointTxStatus(USB_EP2, USB_STAT_TX_NAK);
	/* receive status is ready */
	USB_SetEndpointRxStatus(USB_EP2, USB_STAT_RX_DISABLED);

	/* sync call */
	if (tx_cb == CB_NULL) {
		tx_cb = CB_NONE;
	/* async call was made? */
	} else if (tx_cb != CB_NONE) {
		tx_cb(0);
	}

	/* endpoint 3 - data out */
	/* set endpoint type */
	USB_SetEndpointTypeAddress(USB_EP3, USB_EP_TYPE_BULK, 0x03);
	/* set rx address, just after rx buffer, set max data size */
	USB_SetEndpointRxBuffer(USB_EP3, 0x88, 0x20);
	/* tx status is not ready  */
	USB_SetEndpointTxStatus(USB_EP3, USB_STAT_TX_DISABLED);
	/* receive status is stalled */
	USB_SetEndpointRxStatus(USB_EP3, USB_STAT_RX_NAK);

	/* sync call */
	if (rx_cb == CB_NULL) {
		rx_cb = CB_NONE;
	/* async call was made? */
	} else if (rx_cb != CB_NONE) {
		rx_size = 0;
		rx_cb(&rx_size);
	}
}

/* request callback: handle all special requests */
static void USBVCP_RequestCallback(void *arg)
{
	/* status */
	int rc = EOK;
	/* event argument */
	usb_req_evarg_t *a = arg;
	/* setup frame that cauesd this event */
	usb_setup_t *s = a->setup;

	/* some debug */
	dprintf_i("index = 0x%x, len = %d, r = 0x%x, r_type = 0x%x, val = 0x%x\n",
			s->index, s->length, s->request, s->request_type, s->value);

	/* switch on request type */
	switch (s->request) {
	/* set line encoding */
	case USB_VCP_REQ_SET_LINE_CODING : {
		/* still waiting for the data stage */
		if (a->ptr == 0) {
			/* set where to store data to */
			a->ptr = (void *)&le, a->size = sizeof(le);
		/* got data */
		} else  {
			/* TODO: apply line parameters */
		}
	} break;
	/* get line encoding */
	case USB_VCP_REQ_GET_LINE_CODING : {
		/* set returned data */
		a->ptr = (void *)&le, a->size = sizeof(le);
	} break;
	/* set control line state */
	case USB_VCP_SET_CONTROL_LINE_STATE : {
		/* TODO: implement rts, and dtr signals */
	} break;
	/* unknown command */
	default : {
		/* report an error */
		rc = EFATAL;
	} break;
	}

	/* set status */
	a->status = rc;
}

/* transmission callback */
static void USBVCP_EpTxCallback(void)
{
	/* transfer size */
	uint32_t tsize;

	/* frame sent! */
	if (!tx_size) {
		/* sync call */
		if (tx_cb == CB_NULL) {
			tx_cb = CB_NONE;
		/* async call was made? */
		} else if (tx_cb != CB_NONE) {
			tx_cb(0);
		}
		/* prevent further execution */
		return;
	}

	/* limit transfer size */
	tsize = min(32, tx_size);
	/* write data to endpoint buffer */
	USB_WriteEndpointBuffer(USB_EP2, tx_ptr, tx_size);

	/* update pointer and size */
	tx_ptr += tsize, tx_size -= tsize;
	/* enable transfer */
	USB_SetEndpointTxStatus(USB_EP2, USB_STAT_TX_VALID);
}

/* reception callback */
static void USBVCP_EpRxCallback(void)
{
	/* transfer size */
	rx_size = USB_ReadEndpointBuffer(USB_EP3, rx_ptr, rx_size);

	/* sync call */
	if (rx_cb == CB_NULL) {
		rx_cb = CB_NONE;
	/* async call was made? */
	} else if (rx_cb != CB_NONE) {
		rx_cb(&rx_size);
	}
}

/* endpoint callback - data from device to pc */
static void USBVCP_EpCallback(void *arg)
{
	/* map event */
	usb_ep_evarg_t *a = arg;

	/* ep2 - data from device to host */
	if (a->ep_num == USB_EP2) {
		/* process transmission */
		USBVCP_EpTxCallback();
	/* ep3 - data from Host to Device */
	} else if (a->ep_num == USB_EP3) {
		/* process reception */
		USBVCP_EpRxCallback();
	}
}

/* init hid */
int USBVCP_Init(void)
{
	/* register events */
	Ev_RegisterCallback(&usb_rst_ev, USBVCP_ResetCallback);
	Ev_RegisterCallback(&usb_req_ev, USBVCP_RequestCallback);
	Ev_RegisterCallback(&usb_ep_ev, USBVCP_EpCallback);

	/* report status */
	return EOK;
}

/* write data */
int USBVCP_Send(void *ptr, size_t size, cb_t cb)
{
	/* result code, sync or async call? */
	int rc = size, sync = cb == CB_NULL;

	/* store callback information */
	tx_cb = cb;
	/* store data */
	tx_ptr = ptr, tx_size = size;

	/* this shall start the transfer */
	USBVCP_EpTxCallback();

	/* interrupt will alter callback address after transfer has finished */
	while (sync && tx_cb == CB_NULL);

	/* report size */
	return rc;
}

/* read data */
int USBVCP_Recv(void *ptr, size_t size, cb_t cb)
{
	/* result code, sync or async call? */
	int rc = size, sync = cb == CB_NULL;

	/* store callback information */
	rx_cb = cb;
	/* store data */
	rx_ptr = ptr, rx_size = size;

	/* this shall start the transfer */
	USB_SetEndpointRxStatus(USB_EP3, USB_STAT_RX_VALID);

	/* interrupt will alter callback address after transfer has finished */
	while (sync && rx_cb == CB_NULL);

	/* report size */
	return rc;
}
