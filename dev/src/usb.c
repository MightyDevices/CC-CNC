/*
 * usb.c
 *
 *  Created on: 15-02-2014
 *      Author: Tomek
 */

#include <dev/systime.h>
#include <stm32l1/stm32l1.h>
#include <stm32l1/rcc.h>
#include <stm32l1/nvic.h>
#include <stm32l1/gpio.h>
#include <stm32l1/syscfg.h>
#include <stm32l1/usb.h>
#include <util/minmax.h>
#include <util/string.h>
#include <sys/err.h>
#include <sys/critical.h>
#include <sys/ev.h>
#include <dev/usb.h>
#include <dev/usbdesc.h>

#define DEBUG
#include <dev/debug.h>

/* SETUP stage (wait and process SETUP frame) */
#define USB_CONTROL_SETUP					0x00
/* DATA IN transaction (device is sending data to host) */
#define USB_CONTROL_IN						0x01
/* OUT transaction (host is sending data to device) */
#define USB_CONTROL_OUT						0x03
/* last frame in OUT transaction */
#define USB_CONTROL_OUT_LAST				0x04
/* STATUS OUT (ends control write and control no-data operations) */
#define USB_CONTROL_STATUS_OUT				0x05
/* STATUS IN (ends control read) */
#define USB_CONTROL_STATUS_IN				0x06

/* buffer - used for internal transactions on control pipe */
typedef struct {
	/* buffer */
	uint8_t data[16];
} usb_buf_t;
/* buffer */
static usb_buf_t buf;

/* control endpoint related stuff */
typedef struct {
	/* current setup frame */
	usb_setup_t setup;
	/* current state */
	uint8_t state;
	/* current transfer pointer */
	uint8_t *ptr;
	/* current transfer bytes */
	uint16_t size;
	/* buffer - used for internal transactions on control pipe */
	uint8_t buf[];
} usb_control_t;
/* control endpoint state */
static usb_control_t control;

/* device control block */
typedef struct {
	/* current state */
	uint8_t state;
	/* device address, address update flag */
	uint8_t address, address_update;
	/* device configuration */
	uint8_t configuration;
	/* alternate setting of interfaces (up to 4 are supported) */
	uint8_t alternate_setting[4];
	/* device status mask */
	uint8_t dev_status;
	/* endpoint halt mask for rx and tx */
	uint8_t ep_halt_rx, ep_halt_tx;
} usb_device_t;
/* device control block */
static usb_device_t device;
/* system events: reset event, request event, endpoint event */
ev_t usb_rst_ev, usb_req_ev, usb_ep_ev;

/* buffer table offset */
static void USB_SetBtableOffset(uint16_t offset)
{
	/* set btable offset */
	USB->BTABLE = offset;
}

/* set device address */
static void USB_SetDeviceAddress(uint8_t address)
{
	/* set address and set enable flag */
	USB->DADDR = USB_DADDR_EF | address;
}

/* clear endpoint ctr flag for reception */
static void USB_ClearEnpointCTRRx(uint8_t ep_num)
{
	/* kind field will not be changed */
	uint32_t epr = USB->EPR[ep_num] & (USB_EPR_EA | USB_EPR_EP_KIND |
			USB_EPR_EP_TYPE);

	/* keep tx flag as it is */
	USB->EPR[ep_num] = epr | USB_EPR_CTR_TX;
}

/* clear endpoint ctr flag for transmission */
static void USB_ClearEnpointCTRTx(uint8_t ep_num)
{
	/* kind field will not be changed */
	uint32_t epr = USB->EPR[ep_num] & (USB_EPR_EA | USB_EPR_EP_KIND |
			USB_EPR_EP_TYPE);

	/* keep tx flag as it is */
	USB->EPR[ep_num] = epr | USB_EPR_CTR_RX;
}

/* start data in stage */
static void USB_IsrEp0StartDataInStage(void *ptr, size_t size)
{
	/* get maximal frame size from device descriptor */
	size_t max_size = device_descriptor[7];

	/* store pointer and size */
	control.ptr = ptr, control.size = size;
	/* if no fragmentation is required then set state to IN_LAST */
	control.state = USB_CONTROL_IN;

	/* prepare buffer */
	USB_WriteEndpointBuffer(USB_EP0, ptr, min(size, max_size));
	/* enable transfer */
	USB_SetEndpointTxStatus(USB_EP0, USB_STAT_TX_VALID);
	/* re-enable reception (host may be willing to interrupt data in stage at
	 * some point, windows does it) */
	USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_VALID);
}

/* abort Data In Stage  */
static void USB_IsrEp0AbortDataInStage(void)
{
	/* back to default state */
	control.state = USB_CONTROL_SETUP;
	/* set stall */
	USB_SetEndpointTxStatus(USB_EP0, USB_STAT_RX_STALL);
}

/* start data OUT stage, host should then send us data in single or multiple
 * data packets */
static void USB_IsrEp0StartDataOutStage(void *ptr, size_t size)
{
	/* store pointer and size */
	control.ptr = ptr, control.size = size;
	/* set state */
	control.state = USB_CONTROL_OUT;
	/* enable reception, host shall now send its data */
	USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_VALID);
}

/* abort Data OUT stage, which is used to notify host that whole OUT
 * transaction will not be accepted by device. to accept transmission (and data)
 * use 'USB_IsrEp0StartOutStage' */
static void USB_IsrEp0AbortDataOutStage(void)
{
	/* back to default state */
	control.state = USB_CONTROL_SETUP;
	/* disable reception */
	USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_STALL);
	/* re-enable reception */
	USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_VALID);
}

/* start STATUS stage after Data IN transaction, which is used to notify host
 * that whole IN transaction went smoothly. Used only when there is no data
 * stage (example: set address). in case of transfers with data stage
 * 'USB_IsrEp0StartInStage' is used. */
static void USB_IsrEp0StartStatusInStage(uint16_t stat_tx)
{
	/* status is ACK */
	if (stat_tx == USB_STAT_TX_VALID) {
		/* STATUS IN transfer will take place */
		control.state = USB_CONTROL_STATUS_IN;
		/* prepare buffer (send empty frame) */
		USB_WriteEndpointBuffer(USB_EP0, 0, 0);
	/* status is STALL (meaning error) */
	} else {
		/* back to default state */
		control.state = USB_CONTROL_SETUP;
		/* re-enable reception */
		USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_VALID);
	}

	/* set status */
	USB_SetEndpointTxStatus(USB_EP0, stat_tx);
}

/* process setup set address */
static int USB_IsrEp0ProcessSetupSetAddress(usb_setup_t *s)
{
	/* operation status */
	int rc = EFATAL;
	/* extract recipient */
	uint8_t recipient = s->request_type & USB_SETUP_REQTYPE_RECIPIENT;

	/* check device state */
	if (device.state == USB_DEV_CONFIGURED) {
		/* invalid state */
		return rc;
	}
	/* only device can be addressed */
	if (recipient != USB_SETUP_REQTYPE_RECIPIENT_DEVICE) {
		/* invalid state */
		return rc;
	}

	/* store address. address update will be performed after status
	 * stage (IN transfer) */
	device.address = s->value;
	/* change dev state */
	device.state = device.address == 0 ? USB_DEV_DEFAULT :
			USB_DEV_ADDRESS;
	/* update flag as soon as IN transaction takes place. one
	 * cannot change device's address before IN transaction reaches
	 * end and thats why this flag is used here */
	device.address_update = 1;

	/* all is ok */
	rc = EOK;
	/* report status */
	return rc;
}

/* process setup set configuration */
static int USB_IsrEp0ProcessSetupSetConfiguration(usb_setup_t *s)
{
	/* operation status */
	int rc = EFATAL;
	/* extract recipient */
	uint8_t recipient = s->request_type & USB_SETUP_REQTYPE_RECIPIENT;

	/* check device state */
	if (device.state == USB_DEV_DEFAULT) {
		/* invalid state */
		return rc;
	}
	/* addressed to device */
	if (recipient != USB_SETUP_REQTYPE_RECIPIENT_DEVICE) {
		/* invalid state */
		return rc;
	}
	/* extract configuration */
	device.configuration = s->value;
	/* change dev state */
	device.state = device.configuration == 0 ? USB_DEV_ADDRESS :
			USB_DEV_CONFIGURED;
	/* all is ok */
	rc = EOK;
	/* report status */
	return rc;
}

/* process setup clear feature */
static int USB_IsrEp0ProcessSetupClearFeature(usb_setup_t *s)
{
	/* operation status */
	int rc = EFATAL;
	/* extract recipient */
	uint8_t recipient = s->request_type & USB_SETUP_REQTYPE_RECIPIENT;

	/* extract feature */
	uint16_t feature = s->value;
	/* extract endpoint or iface number */
	uint16_t num = s->index;


	/* device is addressed */
	if (recipient == USB_SETUP_REQTYPE_RECIPIENT_DEVICE) {
		/* device num must be 0 */
		if (num == 0) {
			/* TODO: device remote wakeup: not supported */
			if (feature == USB_SETUP_FEATURE_DEV_REMOTE_WKUP) {
			/* TODO: device test mode: not supported */
			} else if (feature == USB_SETUP_FEATURE_TEST_MODE) {
			}
		}
	/* interface is addressed */
	} else if (recipient == USB_SETUP_REQTYPE_RECIPIENT_IFACE) {
		/* usb spec 2.0 does not define any features for interface */
	} else if (recipient == USB_SETUP_REQTYPE_RECIPIENT_EP) {
		/* endpoint number, endpoint direction */
		uint8_t ep_num = num & 0x7F, ep_dir = num & 0x80;
		/* endpoint halt */
		if (ep_num >= USB_EP1 && ep_num < endpoints_num &&
			feature == USB_SETUP_FEATURE_ENDPOINT_HALT) {
			/* select mask according to ep_dir */
			if (ep_dir) {
				device.ep_halt_tx &= ~(1 << ep_num);
			} else {
				device.ep_halt_rx &= ~(1 << ep_num);
			}
			/* all went ok */
			rc = EOK;
		}
	}


	/* report status */
	return rc;
}

/* process setup set feature */
static int USB_IsrEp0ProcessSetupSetFeature(usb_setup_t *s)
{
	/* operation status */
	int rc = EFATAL;
	/* extract recipient */
	uint8_t recipient = s->request_type & USB_SETUP_REQTYPE_RECIPIENT;

	/* extract feature */
	uint16_t feature = s->value;
	/* extract endpoint or iface number */
	uint16_t num = s->index;

	/* length must be zero */
	if (s->length != 0) {
		return rc;
	}

	/* device is addressed */
	if (recipient == USB_SETUP_REQTYPE_RECIPIENT_DEVICE) {
		/* device num must be 0 */
		if (num == 0) {
			/* TODO: device remote wakeup: not supported */
			if (feature == USB_SETUP_FEATURE_DEV_REMOTE_WKUP) {
			/* TODO: device test mode: not supported */
			} else if (feature == USB_SETUP_FEATURE_TEST_MODE) {
			}
		}
	/* interface is addressed */
	} else if (recipient == USB_SETUP_REQTYPE_RECIPIENT_IFACE) {
		/* usb spec 2.0 does not define any features for interface */
	} else if (recipient == USB_SETUP_REQTYPE_RECIPIENT_EP) {
		/* endpoint number, endpoint direction */
		uint8_t ep_num = num & 0x7F, ep_dir = num & 0x80;
		/* endpoint halt */
		if (ep_num >= USB_EP1 && ep_num < endpoints_num &&
			feature == USB_SETUP_FEATURE_ENDPOINT_HALT) {
			/* select mask according to ep_dir */
			if (ep_dir) {
				device.ep_halt_tx |= (1 << ep_num);
			} else {
				device.ep_halt_rx |= (1 << ep_num);
			}
			/* all went ok */
			rc = EOK;
		}
	}

	/* report status */
	return rc;
}

/* process set interface */
static int USB_IsrEp0ProcessSetupSetInterface(usb_setup_t *s)
{
	/* operation status */
	int rc = EFATAL;
	/* extract recipient */
	uint8_t recipient = s->request_type & USB_SETUP_REQTYPE_RECIPIENT;

	/* interface identifier */
	uint8_t iface_num = s->index;
	/* alternative setting */
	uint8_t iface_alt_num = s->value;

	/* check frame: those fields must have those values */
	if (s->length != 0) {
		return rc;
	}

	/* interface is recipient */
	if (recipient == USB_SETUP_REQTYPE_RECIPIENT_IFACE) {
		/* need to be in configured state */
		if (device.state == USB_DEV_CONFIGURED && iface_num < interfaces_num) {
			/* prepare data */
			device.alternate_setting[iface_num] = iface_alt_num;
			/* status is ok */
			rc = EOK;
		}
	}

	/* report status */
	return rc;
}

/* data */
/* process setup get descriptor */
static int USB_IsrEp0ProcessSetupGetDescriptor(usb_setup_t *s, void **ptr,
		size_t *size)
{
	/* operation status */
	int rc = EFATAL;
	/* extract recipient */
	uint8_t recipient = s->request_type & USB_SETUP_REQTYPE_RECIPIENT;

	/* addressed to device */
	if (recipient == USB_SETUP_REQTYPE_RECIPIENT_DEVICE) {
		/* what descriptor type is requested? */
		uint8_t desc_type = s->value >> 8;
		/* descriptor index */
		uint8_t desc_index = s->value;

		/* assume that everything is ok */
		rc = EOK;

		/* prepare data buffer */
		switch (desc_type) {
		/* device descriptor was requested */
		case USB_SETUP_DESCTYPE_DEVICE : {
			/* set data pointer and size */
			*ptr = (uint8_t *)device_descriptor;
			*size = (uint8_t)device_descriptor[0];
		} break;
		/* configuration descriptor was requested */
		case USB_SETUP_DESCTYPE_CONFIGURATION : {
			/* check if descriptor exists */
			if (desc_index >= config_descriptors_num) {
				/* wrong index */
				break;
			}
			/* set data pointer and size */
			*ptr = (uint8_t *)config_descriptors[desc_index];
			*size = config_descriptors[desc_index][2] |
					config_descriptors[desc_index][3] << 8;
		} break;
		/* string descriptor was requested */
		case USB_SETUP_DESCTYPE_STRING : {
			/* check if descriptor exists */
			if (desc_index >= string_descriptors_num) {
				/* wrong index */
				break;
			}
			/* set data pointer and size */
			*ptr = (uint8_t *)string_descriptors[desc_index];
			*size = (uint8_t)string_descriptors[desc_index][0];
		} break;
		/* unknown descriptor */
		default : {
			/* report an error */
			rc = EFATAL;
		} break;
		}
	/* addressed to interface */
	} else if (recipient == USB_SETUP_REQTYPE_RECIPIENT_IFACE) {
		/* argument */
		usb_req_evarg_t arg = {s, EFATAL, 0, 0};
		/* all other modules */
		Ev_Notify(&usb_req_ev, &arg);
		/* copy status, pointer and size */
		rc = arg.status, *ptr = arg.ptr, *size = arg.size;
	}

	/* limit size, host should only get as many bytes as it requests, no matter
	 * if data will be truncated */
	*size = min(*size, s->length);
	/* report status */
	return rc;
}

/* process get configuration */
static int USB_IsrEp0ProcessSetupGetConfiguration(usb_setup_t *s, void **ptr,
		size_t *size)
{
	/* operation status */
	int rc = EFATAL;
	/* extract recipient */
	uint8_t recipient = s->request_type & USB_SETUP_REQTYPE_RECIPIENT;

	/* addressed to device */
	if (recipient == USB_SETUP_REQTYPE_RECIPIENT_DEVICE) {
		/* check frame */
		if (s->value == 0 && s->index == 0 && s->length == 1) {
			/* check our state */
			if (device.state != USB_DEV_DEFAULT) {
				/* prepare data */
				*ptr = &device.configuration;
				*size = 1;
				/* report success */
				rc = EOK;
			}
		}
	}

	/* report status */
	return rc;
}

/* process get status */
static int USB_IsrEp0ProcessSetupGetStatus(usb_setup_t *s, void **ptr,
		size_t *size)
{
	/* operation status */
	int rc = EFATAL;
	/* extract recipient */
	uint8_t recipient = s->request_type & USB_SETUP_REQTYPE_RECIPIENT;

	/* check frame: those fields must have those values */
	if (s->value != 0 || s->length != 2) {
		return rc;
	}

	/* addressed to device */
	if (recipient == USB_SETUP_REQTYPE_RECIPIENT_DEVICE) {
		/* check frame */
		if (s->index == 0) {
			/* check our state */
			if (device.state != USB_DEV_DEFAULT) {
				/* prepare data in temporary buffer: two bits: self-powered,
				 * remote wake-up */
				buf.data[0] = device.dev_status; buf.data[1] = 0;
				/* prepare transfer */
				*ptr = buf.data;
				*size = 2;
				/* report success */
				rc = EOK;
			}
		}
	/* interface is recipient */
	} else if (recipient == USB_SETUP_REQTYPE_RECIPIENT_IFACE) {
		/* interface number */
		uint8_t iface_num = s->index;
		/* request is supported only in configured state */
		if (device.state == USB_DEV_CONFIGURED && iface_num < interfaces_num) {
			/* prepare data in temporary buffer: both zeros */
			buf.data[0] = 0; buf.data[1] = 0;
			/* prepare transfer */
			*ptr = buf.data;
			*size = 2;
			/* report success */
			rc = EOK;
		}
	/* endpoint is recipient */
	} else if (recipient == USB_SETUP_REQTYPE_RECIPIENT_EP) {
		/* get endpoint number, and endpoint direction (7-bit, when set then we
		 * are talking about IN (tx) endpoint */
		uint8_t ep_num = s->index & 0x7F, ep_dir = s->index & 0x80;
		/* endpoint 0 can be addressed in address state, all others can be
		 * addressed in configured state */
		if ((ep_num == USB_EP0 && device.state != USB_DEV_DEFAULT) ||
			 (ep_num > USB_EP0 && ep_num < endpoints_num &&
					 device.state == USB_DEV_CONFIGURED)) {
			/* get proper endpoint mask */
			uint8_t ep_halt = ep_dir ? device.ep_halt_tx : device.ep_halt_rx;
			/* prepare data in temporary buffer: one bit - halt status */
			buf.data[0] = ep_halt & (1 << ep_num) ? 1 : 0;
			buf.data[1] = 0;
			/* prepare transfer */
			*ptr = buf.data;
			*size = 2;
			/* report success */
			rc = EOK;
		}
	}

	/* report status */
	return rc;
}

/* process get interface */
static int USB_IsrEp0ProcessSetupGetInterface(usb_setup_t *s, void **ptr,
		size_t *size)
{
	/* operation status */
	int rc = EFATAL;
	/* extract recipient */
	uint8_t recipient = s->request_type & USB_SETUP_REQTYPE_RECIPIENT;
	/* interface identifier */
	uint8_t iface_num = s->index;

	/* check frame: those fields must have those values */
	if (s->value != 0 || s->length != 1) {
		return rc;
	}

	/* interface is recipient */
	if (recipient == USB_SETUP_REQTYPE_RECIPIENT_IFACE) {
		/* need to be in configured state */
		if (device.state == USB_DEV_CONFIGURED && iface_num < interfaces_num) {
			/* prepare data */
			buf.data[0] = device.alternate_setting[iface_num];
			/* prepare transfer */
			*ptr = buf.data;
			*size = 1;
			/* status is ok */
			rc = EOK;
		}
	}

	/* report status */
	return rc;
}

/* process out tokens */
static void USB_IsrEp0ProcessOut(void)
{
	/* waiting in STATUS after Data IN stage? */
	if (control.state == USB_CONTROL_STATUS_OUT) {
		/* device has transmitted all the data successfully */
		control.state = USB_CONTROL_SETUP;
		/* re-enable reception */
		USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_VALID);
	/* got OUT while sending data? this means that host aborted current
	 * transfer */
	} else if (control.state == USB_CONTROL_IN) {
		/* end of processing */
		control.state = USB_CONTROL_SETUP;
		/* disable transfer */
		USB_SetEndpointTxStatus(USB_EP0, USB_STAT_TX_STALL);
		/* re-enable reception */
		USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_VALID);
	/* handle incoming data */
	} else if (control.state == USB_CONTROL_OUT) {
		/* read data from endpoint */
		int size = USB_ReadEndpointBuffer(USB_EP0, control.ptr, control.size);
		/* update pointer & size */
		control.ptr += size; control.size -= size;
		/* check if more frames are to be expected, if no then go to STATUS OUT
		 * stage */
		if (control.size == 0) {
			/* prepare callback argument */
			usb_req_evarg_t arg = {&control.setup, EFATAL, &control.ptr, 0};
			/* call event */
			Ev_Notify(&usb_req_ev, &arg);
			/* data processed ok? */
			if (arg.status == EOK) {
				USB_IsrEp0StartStatusInStage(USB_STAT_TX_VALID);
			/* fail */
			} else {
				USB_IsrEp0StartStatusInStage(USB_STAT_TX_STALL);
			}
		/* re-enable reception, more frames will come */
		} else {
			USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_VALID);
		}
	}
}

/* process IN transfer */
static void USB_IsrEp0ProcessIn(void)
{
	/* still some frames to send? */
	if (control.state == USB_CONTROL_IN) {
		/* get maximal frame size from device descriptor */
		size_t max_size = device_descriptor[7];
		/* still some data to send? */
		if (control.size >= max_size) {
			/* update pointers */
			control.size -= max_size, control.ptr += max_size;
			/* limit size */
			max_size = min(control.size, max_size);
			/* prepare buffer */
			USB_WriteEndpointBuffer(USB_EP0, control.ptr, max_size);
			/* enable transfer */
			USB_SetEndpointTxStatus(USB_EP0, USB_STAT_TX_VALID);
		/* end of data in stage */
		} else {
			/* wait for status frame (host acknowledges with OUT frame) */
			control.state = USB_CONTROL_STATUS_OUT;
		}
	/* STATUS IN received */
	} else if (control.state == USB_CONTROL_STATUS_IN) {
		/* end of processing */
		control.state = USB_CONTROL_SETUP;
		/* re-enable reception */
		USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_VALID);
		/* this is a special handler for address change */
		if (device.address_update) {
			/* update address */
			USB_SetDeviceAddress(device.address);
			/* clear flag */
			device.address_update = 0;
		}
	}
}

/* process setup frame with no data stage */
static void USB_IsrEp0ProcessSetupNoData(void)
{
	/* status of frame processing */
	int rc = EFATAL;
	/* setup frame pointer */
	usb_setup_t *s = &control.setup;
	/* extract type */
	uint8_t type = s->request_type & USB_SETUP_REQTYPE_TYPE;

	/* standard request */
	if (type == USB_SETUP_REQTYPE_TYPE_STANDARD) {
		/* process all 'set' frames. such frames have direction bit cleared */
		if (!(s->request_type & USB_SETUP_REQTYPE_DIR)) {
			/* frame opcode is contained in 'request' field */
			switch (s->request) {
			/* set device address */
			case USB_SETUP_REQ_SET_ADDRESS : {
				/* process frame */
				rc = USB_IsrEp0ProcessSetupSetAddress(s);
			} break;
			/* set configuration */
			case USB_SETUP_REQ_SET_CONFIGURATION : {
				/* process frame */
				rc = USB_IsrEp0ProcessSetupSetConfiguration(s);
			} break;
			/* clear feature */
			case USB_SETUP_REQ_CLEAR_FEATURE : {
				/* process frame */
				rc = USB_IsrEp0ProcessSetupClearFeature(s);
			} break;
			/* set feature */
			case USB_SETUP_REQ_SET_FEATURE : {
				/* process frame */
				rc = USB_IsrEp0ProcessSetupSetFeature(s);
			} break;
			/* set interface */
			case USB_SETUP_REQ_SET_INTERFACE : {
				rc = USB_IsrEp0ProcessSetupSetInterface(s);
			} break;
			/* unsupported frame? */
			default : {
				/* process frame */
				usb_req_evarg_t arg = {s, EFATAL, 0, 0};
				/* call event */
				Ev_Notify(&usb_req_ev, &arg);
				/* copy status */
				rc = arg.status;
			} break;
			}
		}
	/* class request, vendor request - all wont be handled by this code, so
	 * redirect those to upper layers */
	} else {
		/* process frame */
		usb_req_evarg_t arg = {s, EFATAL, 0, 0};
		/* call event */
		Ev_Notify(&usb_req_ev, &arg);
		/* copy status */
		rc = arg.status;
	}

	/* status reported depends on proper/improper frame processing */
	int stat_tx = rc == EOK ? USB_STAT_TX_VALID : USB_STAT_TX_STALL;
	/* set status */
	USB_IsrEp0StartStatusInStage(stat_tx);
}

/* process setup frame with data stage */
static void USB_IsrEp0ProcessSetupData(void)
{
	/* status of frame processing */
	int rc = EFATAL;
	/* setup frame pointer */
	usb_setup_t *s = &control.setup;
	/* extract type */
	uint8_t type = s->request_type & USB_SETUP_REQTYPE_TYPE;
	/* event for class specific requests */
	usb_req_evarg_t arg = {s, EFATAL, 0, 0};

	/* data pointer */
	void *ptr = 0;
	/* data size */
	size_t size = 0;

	/* standard request */
	if (type == USB_SETUP_REQTYPE_TYPE_STANDARD) {
		/* process all 'get' frames. such frames have direction bit set. device
		 * shall respond with data after parsing frame */
		if (s->request_type & USB_SETUP_REQTYPE_DIR) {

			/* frame opcode is contained in 'request' field */
			switch (s->request) {
			/* get descriptor request. this may be addressed only to device
			 * itself this frame is used to obtain device, configuration,
			 * interface, endpoint and interface descriptor */
			case USB_SETUP_REQ_GET_DESCRIPTOR : {
				/* process frame */
				rc = USB_IsrEp0ProcessSetupGetDescriptor(s, &ptr, &size);
			} break;
			/* get configuration */
			case USB_SETUP_REQ_GET_CONFIGURATION : {
				/* process frame */
				rc = USB_IsrEp0ProcessSetupGetConfiguration(s, &ptr, &size);
			} break;
			/* get status */
			case USB_SETUP_REQ_GET_STATUS : {
				/* process frame */
				rc = USB_IsrEp0ProcessSetupGetStatus(s, &ptr, &size);
			} break;
			/* get interface */
			case USB_SETUP_REQ_GET_INTERFACE : {
				/* process frame */
				rc = USB_IsrEp0ProcessSetupGetInterface(s, &ptr, &size);
			} break;
			/* unsupported frame? */
			default : {
				/* call event */
				Ev_Notify(&usb_req_ev, &arg);
				/* copy status & other stuff */
				rc = arg.status, ptr = arg.ptr, size = arg.size;
			} break;
			}
		}
	/* class request, vendor request - those won't be handled by this code, so
	 * redirect those to upper layers */
	} else {
		/* call event */
		Ev_Notify(&usb_req_ev, &arg);
		/* copy status & other stuff */
		rc = arg.status, ptr = arg.ptr, size = arg.size;
	}

	/* prepare data stage according to transfer direction and result code */
	/* data is to be sent from device to host */
	if (s->request_type & USB_SETUP_REQTYPE_DIR) {
		/* frame processed successfully? */
		if (rc == EOK) {
			/* data in stage */
			USB_IsrEp0StartDataInStage(ptr, size);
		/* an error has occurred */
		} else {
			/* send STALL status */
			USB_IsrEp0AbortDataInStage();
		}
	/* data will be sent from host to device */
	} else {
		/* frame processed successfully? */
		if (rc == EOK) {
			/* initiate OUT transfer */
			USB_IsrEp0StartDataOutStage(ptr, size);
		/* an error has occurred */
		} else {
			/* abort transaction */
			USB_IsrEp0AbortDataOutStage();
		}
	}
}

/* process setup frame */
static void USB_IsrEp0ProcessSetup(void)
{
	/* transfer frame from packet memory to user memory */
	USB_ReadEndpointBuffer(USB_EP0, &control.setup, sizeof(usb_setup_t));
	/* some debug */
	dprintf_i("type = %x, req = %x, val = %x, idx = %x, len = %x\n",
			control.setup.request_type, control.setup.request,
			control.setup.value, control.setup.index,
			control.setup.length);

	/* no data is to be transmitted in any way */
	if (control.setup.length == 0) {
		/* process setup frame with no data stage */
		USB_IsrEp0ProcessSetupNoData();
	/* data stage is about to take place */
	} else {
		/* process setup frame with data stage */
		USB_IsrEp0ProcessSetupData();
	}
}

/* handle correct transfer interrupt */
static void USB_IsrCorrectTransfer(void)
{
	/* to avoid funny business (a.k.a race-conditions) we use the same istr
	 * value during single iteration */
	uint16_t istr = USB->ISTR;
	/* get endpoint number */
	uint16_t ep_num = istr & USB_ISTR_EP_ID;
	/* get epr register */
	uint16_t epr = USB->EPR[ep_num];
	/* event argument for notifying upper layers */
	usb_ep_evarg_t arg = {ep_num, 0};

	/* control endpoint */
	if (ep_num == USB_EP0) {
		/* check transfer direction, if dir is clear, then this is IN
		 * transaction */
		if ((istr & USB_ISTR_DIR) == 0) {
			/* clear tx flag */
			USB_ClearEnpointCTRTx(ep_num);
			/* process frame */
			USB_IsrEp0ProcessIn();
		/* SETUP transaction */
		} else if (epr & USB_EPR_SETUP) {
			/* clear rx flag */
			USB_ClearEnpointCTRRx(ep_num);
			/* process setup transaction */
			USB_IsrEp0ProcessSetup();
		/* OUT transaction */
		} else {
			/* clear rx flag */
			USB_ClearEnpointCTRRx(ep_num);
			/* process frame */
			USB_IsrEp0ProcessOut();
		}
	/* all other endpoints go here */
	} else {
		/* data received? */
		if (epr & USB_EPR_CTR_RX) {
			/* clear tx flag */
			USB_ClearEnpointCTRRx(ep_num);
			/* set type */
			arg.type = USB_EP_RX;
			/* call */
			Ev_Notify(&usb_ep_ev, &arg);
		}

		/* data transmitted? */
		if (epr & USB_EPR_CTR_TX) {
			/* clear tx flag */
			USB_ClearEnpointCTRTx(ep_num);
			/* set type */
			arg.type = USB_EP_TX;
			/* call */
			Ev_Notify(&usb_ep_ev, &arg);
		}
	}
}

/* handle usb reset event */
static void USB_IsrReset(void)
{
	/* reset btable offset */
	USB_SetBtableOffset(0x00);

	/* endpoint 0 - Control enpoint */
	/* set endpoint type and address */
	USB_SetEndpointTypeAddress(USB_EP0, USB_EP_TYPE_CONTROL, 0x00);
	/* set rx address */
	USB_SetEndpointRxBuffer(USB_EP0, 0x40, 0x20);
	/* set tx address, just after rx buffer, do not set data count */
	USB_SetEndpointTxBuffer(USB_EP0, 0x60, 0);
	/* tx status is stall by default for control endpoint  */
	USB_SetEndpointTxStatus(USB_EP0, USB_STAT_TX_STALL);
	/* receive status is ready */
	USB_SetEndpointRxStatus(USB_EP0, USB_STAT_RX_VALID);

	/* reset callback */
	Ev_Notify(&usb_rst_ev, 0);

	/* Set this device to respond on default address */
	USB_SetDeviceAddress(0x00);
	/* reset control endpoint */
	memset(&control, 0, sizeof(control));
	/* reset device control block */
	memset(&device, 0, sizeof(device));
}

/* interrupt service routine */
void USB_LPIrq(void)
{
	/* error bit */
	bitband_t *err = STM32_BB_PERIPH(&USB->ISTR, 13);
	/* reset bit */
	bitband_t *reset = STM32_BB_PERIPH(&USB->ISTR, 10);
	/* ctr bit */
	bitband_t *ctr = STM32_BB_PERIPH(&USB->ISTR, 15);

	/* correct transfer */
	if (*ctr) {
		/* handle whatever was transferred */
		USB_IsrCorrectTransfer();
	}

	/* bus reset occured */
	if (*reset) {
		/* interrupt service routine */
		USB_IsrReset();
		/* clear interrupt */
		*reset = 0;
	}

	/* bus error */
	if (*err) {
		/* clear interrupt */
		*err = 0;
	}
}

/* initialize usb device */
int USB_Init(void)
{
	/* enter critical section */
	Critical_Enter();

	/* enable gpios */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	/* enable sys config */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	/* enable usb */
	RCC->APB1ENR |= RCC_APB1ENR_USBEN;

	/* configure gpios */
	GPIOA->MODER &= ~(GPIO_MODER_MODER11 | GPIO_MODER_MODER12);

	/* set priority */
	NVIC_SETINTPRI(STM32_INT_USBLP, 0x10);
	/* enable */
	NVIC_ENABLEINT(STM32_INT_USBLP);

	/* connect cable */
	SYSCFG->PMC |= SYSCFG_PMC_USB_PU;

	/* exit critical section */
	Critical_Exit();

	/* not much to fail here */
	return EOK;
}

/* perform usb reset */
void USB_Reset(void)
{
	/* force usb reset, exit power down */
	USB->CNTR = USB_CNTR_FRES;
	/* release reset */
	USB->CNTR = 0;
	/* clear interrupts */
	USB->ISTR = 0;
	/* enable interrupts for reset end, and correct transfer */
	USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM;
}

/* set endpoint address and type */
void USB_SetEndpointTypeAddress(int ep_num, int type, int address)
{
	/* kind field will not be changed */
	uint32_t epr = USB->EPR[ep_num] & USB_EPR_EP_KIND;

	/* set type and address, but do not touch any other stuff */
	USB->EPR[ep_num] = USB_EPR_MASK | type | address | epr;
}

/* set endpoint receive buffer address & size */
void USB_SetEndpointRxBuffer(int ep_num, int offset, size_t size)
{
	/* get btable entry */
	usb_btable_t *b = USB_BTABLE(ep_num);

	/* set rx buffer */
	b->ADDR_RX = offset;

	/* if size is greater than 32 bytes then it needs to be encoded */
	if (size >= 32) {
		/* size is expressed in 32 byte blocks */
		size = (size >> 5) - 1;
		/* set block size flag */
		b->COUNT_RX = USB_COUNT_RX_BLSIZE | size << 10;
	/* normally size is expressed in two byte blocks */
	} else {
		/* size is expressed in 2 byte blocks */
		size = size >> 1;
		/* do not set block size flag */
		b->COUNT_RX = size << 10;
	}
}

/* set endpoint transmit buffer address & size */
void USB_SetEndpointTxBuffer(int ep_num, int offset, size_t size)
{
	/* get btable entry */
	usb_btable_t *b = USB_BTABLE(ep_num);

	/* set tx buffer */
	b->ADDR_TX = offset;
	/* store size */
	b->COUNT_TX = size;
}

/* set endpoint status (used for marking buffer as ready-to-receive) */
void USB_SetEndpointRxStatus(int ep_num, uint16_t status)
{
	/* get current stat rx state */
	uint32_t epr = USB->EPR[ep_num] & (USB_EPR_EA | USB_EPR_EP_KIND |
			USB_EPR_EP_TYPE | USB_EPR_STAT_RX);

	/* toggle */
	USB->EPR[ep_num] = USB_EPR_MASK | (epr ^ status);
}

/* set endpoint status (used for marking buffer as ready-to-send) */
void USB_SetEndpointTxStatus(int ep_num, uint16_t status)
{
	/* get current stat tx state */
	uint32_t epr = USB->EPR[ep_num] & (USB_EPR_EA | USB_EPR_EP_KIND |
			USB_EPR_EP_TYPE | USB_EPR_STAT_TX);

	/* toggle */
	USB->EPR[ep_num] = USB_EPR_MASK | (epr ^ status);
}

/* read from endpoint rx buffer, ptr must be uint16_t aligned, max size must be
 * even */
int USB_ReadEndpointBuffer(int ep_num, void *ptr, size_t size)
{
	/* set source (pma) address */
	uint16_t *dst = ptr;
	uint32_t *src = (uint32_t *)(USB_BTABLE_BASE +
			USB_BTABLE(ep_num)->ADDR_RX * 2);

	/* limit data size */
	size = min(USB_BTABLE(ep_num)->COUNT_RX & USB_COUNT_RX_COUNT_RX, size);
	/* number of transfers */
	uint32_t cnt = (size + 1) / 2;
	/* copy data */
	while (cnt--)
		*(dst++) = *(src++);

	/* report size */
	return size;
}

/* write to endpoint tx buffer */
int USB_WriteEndpointBuffer(int ep_num, void *ptr, size_t size)
{
	/* set source (pma) address */
	uint32_t *dst = (uint32_t *)(USB_BTABLE_BASE +
			USB_BTABLE(ep_num)->ADDR_TX * 2);
	uint16_t *src = ptr;

	/* number of transfers */
	uint32_t cnt = (size + 1) / 2;
	/* copy data */
	while (cnt--)
		*(dst++) = *(src++);

	/* set counter */
	USB_BTABLE(ep_num)->COUNT_TX = size;
	/* report size */
	return size;
}
