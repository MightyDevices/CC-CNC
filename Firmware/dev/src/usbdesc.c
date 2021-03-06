/*
 * usbhid.c
 *
 *  Created on: 16-02-2014
 *  Author: Tomek
 */

#include <stdint.h>
#include <dev/usbdesc.h>

/* USB Configuration Descriptor */
static const uint8_t config0_descriptor[0x43] = {
		/* Configuration Descriptor*/
		0x09,   		/* bLength: Configuration Descriptor size */
		0x02,  			/* bDescriptorType: Configuration */
		0x43, 0x00, 	/* wTotalLength: no of returned bytes */
		0x02,   		/* bNumInterfaces: 2 interfaces */
		0x01,   		/* bConfigurationValue: Configuration value */
		0x00,   		/* iConfiguration: Index of string descriptor describing
						   the configuration */
		0xC0,   		/* bmAttributes: self powered */
		0x32,   		/* MaxPower 0 mA */

		/* Interface Descriptor */
		0x09,   		/* bLength: Interface Descriptor size */
		0x04, 	 		/* bDescriptorType: Interface descriptor type  */
		0x00,   		/* bInterfaceNumber: Number of Interface */
		0x00,   		/* bAlternateSetting: Alternate setting */
		0x01,   		/* bNumEndpoints: One endpoints used */
		0x02,   		/* bInterfaceClass: Communication Interface Class */
		0x02,   		/* bInterfaceSubClass: Abstract Control Model */
		0x01,   		/* bInterfaceProtocol: Common AT commands */
		0x00,   		/* iInterface: */

		/* Header Functional Descriptor */
		0x05,			/* bLength: Endpoint Descriptor size */
		0x24,			/* bDescriptorType: CS_INTERFACE */
		0x00,			/* bDescriptorSubtype: Header Func Desc */
		0x10, 0x01,		/* bcdCDC: spec release number */

		/* Union Functional Descriptor */
		0x05,			/* bFunctionLength */
		0x24,			/* bDescriptorType: CS_INTERFACE */
		0x06,			/* bDescriptorSubtype: Union func desc */
		0x00,			/* bMasterInterface: Communication class interface */
		0x01,			/* bSlaveInterface0: Data Class Interface */

		/* Call Management Functional Descriptor */
		0x05,			/* bFunctionLength */
		0x24,			/* bDescriptorType: CS_INTERFACE */
		0x01,			/* bDescriptorSubtype: Call Management Func Desc */
		0x00,			/* bmCapabilities: D0+D1 */
		0x01,			/* bDataInterface: 1 */

		/* ACM Functional Descriptor */
		0x04,			/* bFunctionLength */
		0x24,			/* bDescriptorType: CS_INTERFACE */
		0x02,			/* bDescriptorSubtype: Abstract Control Management
						   desc */
		0x02,			/* bmCapabilities */

		/* Endpoint 1 Descriptor */
		0x07,			/* bLength: Endpoint Descriptor size */
		0x05,			/* bDescriptorType: Endpoint */
		0x81,			/* bEndpointAddress: (IN1) */
		0x03,			/* bmAttributes: Interrupt */
		0x08, 0x00,		/* wMaxPacketSize: */
		0xFF,			/* bInterval: */

		/* Data class interface descriptor */
		0x09,			/* bLength: Endpoint Descriptor size */
		0x04,			/* bDescriptorType: */
		0x01,			/* bInterfaceNumber: Number of Interface */
		0x00,			/* bAlternateSetting: Alternate setting */
		0x02,			/* bNumEndpoints: Two endpoints used */
		0x0A,			/* bInterfaceClass: Data Interface Class */
		0x00,			/* bInterfaceSubClass: */
		0x00,			/* bInterfaceProtocol: */
		0x00,			/* iInterface: */

		/* Endpoint 2 Descriptor */
		0x07,			/* bLength: Endpoint Descriptor size */
		0x05,			/* bDescriptorType: Endpoint */
		0x82,			/* bEndpointAddress: (IN2) */
		0x02,			/* bmAttributes: Bulk */
		0x20, 0x00,		/* wMaxPacketSize: */
		0x00,			/* bInterval: ignore for Bulk transfer */

		/* Endpoint 3 Descriptor */
		0x07,			/* bLength: Endpoint Descriptor size */
		0x05,			/* bDescriptorType: Endpoint */
		0x03,			/* bEndpointAddress: (OUT3) */
		0x02,			/* bmAttributes: Bulk */
		0x20, 0x00,		/* wMaxPacketSize: */
		0x00,			/* bInterval: ignore for Bulk transfer */
};

/* language ID */
static const uint8_t string0_descritptor[] = {
		0x04,					/* bLength */
		0x03,					/* bDescriptorType */
		0x09, 0x04,				/* LangID = 0x0409: U.S. English */
};

/* vendor string */
static const uint8_t string1_descritptor[] = {
		0x1E,					/* bLength */
		0x03,					/* bDescriptorType */
		'M', 0, 'i', 0,
		'g', 0, 'h', 0,
		't', 0, 'y', 0,
		' ', 0, 'D', 0,
		'e', 0, 'v', 0,
		'i', 0, 'c', 0,
		'e', 0, 's', 0
};

/* product string */
static const uint8_t string2_descritptor[] = {
		0x0E,					/* bLength */
		0x03,					/* bDescriptorType */
		'C', 0, 'C', 0,
		'-', 0, 'C', 0,
		'N', 0, 'C', 0,
};

/* usb standard device descriptor */
const uint8_t device_descriptor[] = {
		0x12,					/* bLength */
		0x01,					/* bDescriptorType */
		0x00, 0x02, 			/* bcdUSB */
		0x02,					/* bDeviceClass: Communication Class Device */
		0x00,					/* bDeviceSubClass */
		0x00,					/* bDeviceProtocol */
		0x40,					/* bMaxPacketSize40 */
		0x83, 0x04,				/* idVendor (0x0483) */
		0x40, 0x57,				/* idProduct = (0x5740) */
		0x00, 0x02,				/* bcdDevice rel. 2.00 */
		0x01,					/* Index of string descriptor describing
								 * manufacturer */
		0x02,					/* Index of string descriptor describing
								 * product */
		0x00,					/* Index of string descriptor describing the
								 * device serial number */
		0x01					/* bNumConfigurations */
};

/* array of string descriptors */
const uint8_t * config_descriptors[] = {
		config0_descriptor,
};

/* array of string descriptors */
const uint8_t * string_descriptors[] = {
		string0_descritptor,
		string1_descritptor,
		string2_descritptor,
};

/* number of descriptors */
/* configuration descriptors number */
const uint8_t config_descriptors_num = 1;
/* string descriptors number */
const uint8_t string_descriptors_num = 3;
/* number of interfaces */
const uint8_t interfaces_num = 2;
/* number of used endpoints */
const uint8_t endpoints_num = 3;

