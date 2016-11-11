/*
 * usb.h
 *
 *  Created on: 31-03-2013
 *      Author: Tomek
 */

#ifndef STM32L1_USB_H_
#define STM32L1_USB_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define USB_BASE							(0x40005C00)
/* usb */
#define USB									((usb_t *)USB_BASE)

/* usb registers */
typedef struct {
	reg32_t EPR[8];
	reg32_t RESERVED[8];
	reg32_t CNTR;
	reg32_t ISTR;
	reg32_t FNR;
	reg32_t DADDR;
	reg32_t BTABLE;
} usb_t;

/* USB endpoint n register */
#define USB_EPR_EA                         	0x000F
#define USB_EPR_STAT_TX                    	0x0030
#define USB_EPR_STAT_TX_0                 	0x0010
#define USB_EPR_STAT_TX_1                  	0x0020
#define USB_EPR_DTOG_TX                    	0x0040
#define USB_EPR_CTR_TX                     	0x0080
#define USB_EPR_EP_KIND                    	0x0100
#define USB_EPR_EP_TYPE                    	0x0600
#define USB_EPR_EP_TYPE_0                  	0x0200
#define USB_EPR_EP_TYPE_1                  	0x0400
#define USB_EPR_SETUP                      	0x0800
#define USB_EPR_STAT_RX                    	0x3000
#define USB_EPR_STAT_RX_0                  	0x1000
#define USB_EPR_STAT_RX_1                  	0x2000
#define USB_EPR_DTOG_RX                    	0x4000
#define USB_EPR_CTR_RX                     	0x8000
#define USB_EPR_MASK						0x8080


/* USB control register */
#define USB_CNTR_FRES                       0x0001
#define USB_CNTR_PDWN                       0x0002
#define USB_CNTR_LP_MODE                    0x0004
#define USB_CNTR_FSUSP                      0x0008
#define USB_CNTR_RESUME                     0x0010
#define USB_CNTR_ESOFM                      0x0100
#define USB_CNTR_SOFM                       0x0200
#define USB_CNTR_RESETM                     0x0400
#define USB_CNTR_SUSPM                      0x0800
#define USB_CNTR_WKUPM                      0x1000
#define USB_CNTR_ERRM                       0x2000
#define USB_CNTR_PMAOVRM                    0x4000
#define USB_CNTR_CTRM                       0x8000

/* USB interrupt status register */
#define USB_ISTR_EP_ID                      0x000F
#define USB_ISTR_DIR                        0x0010
#define USB_ISTR_ESOF                       0x0100
#define USB_ISTR_SOF                        0x0200
#define USB_ISTR_RESET                      0x0400
#define USB_ISTR_SUSP                       0x0800
#define USB_ISTR_WKUP                       0x1000
#define USB_ISTR_ERR                        0x2000
#define USB_ISTR_PMAOVR                     0x4000
#define USB_ISTR_CTR                        0x8000

/* USB frame number register */
#define USB_FNR_FN                          0x07FF
#define USB_FNR_LSOF                        0x1800
#define USB_FNR_LCK                         0x2000
#define USB_FNR_RXDM                        0x4000
#define USB_FNR_RXDP                        0x8000

/* USB device address */
#define USB_DADDR_ADD                       0x7F
#define USB_DADDR_ADD0                      0x01
#define USB_DADDR_ADD1                      0x02
#define USB_DADDR_ADD2                      0x04
#define USB_DADDR_ADD3                      0x08
#define USB_DADDR_ADD4                      0x10
#define USB_DADDR_ADD5                      0x20
#define USB_DADDR_ADD6                      0x40
#define USB_DADDR_EF                        0x80

/* usb count rx */
#define USB_COUNT_RX_COUNT_RX              	0x03FF
/* number of blocks */
#define USB_COUNT_RX_NUM_BLOCK            	0x7C00
#define USB_COUNT_RX_NUM_BLOCK_0          	0x0400
#define USB_COUNT_RX_NUM_BLOCK_1          	0x0800
#define USB_COUNT_RX_NUM_BLOCK_2          	0x1000
#define USB_COUNT_RX_NUM_BLOCK_3          	0x2000
#define USB_COUNT_RX_NUM_BLOCK_4          	0x4000
/* block size */
#define USB_COUNT_RX_BLSIZE               	0x8000

/* usb count tx */
#define USB_COUNT_TX_COUNT_TX          		0x03FF

/* usb endpoint rx status */
#define USB_STAT_RX_DISABLED				0x0000
#define USB_STAT_RX_STALL					0x1000
#define USB_STAT_RX_NAK						0x2000
#define USB_STAT_RX_VALID					0x3000

/* usb endpoint tx status */
#define USB_STAT_TX_DISABLED				0x0000
#define USB_STAT_TX_STALL					0x0010
#define USB_STAT_TX_NAK						0x0020
#define USB_STAT_TX_VALID					0x0030

/* usb endpoint types */
#define USB_EP_TYPE_BULK					(0x00 << 9)
#define USB_EP_TYPE_CONTROL					(0x01 << 9)
#define USB_EP_TYPE_ISO						(0x02 << 9)
#define USB_EP_TYPE_INT						(0x03 << 9)

/* usb buffer descriptor table */
typedef struct {
	reg32_t ADDR_TX;
	reg32_t COUNT_TX;
	reg32_t ADDR_RX;
	reg32_t COUNT_RX;
} usb_btable_t;

/* register base */
#define USB_BTABLE_BASE						0x40006000
/* usb btable */
#define USB_BTABLE(x)						((usb_btable_t *)(USB_BTABLE_BASE \
											 + x * 16))
/* usb pma address */
#define USB_PMA_ADDR						((void *)(USB_BTABLE_BASE))


#endif /* STM32L1_USB_H_ */
