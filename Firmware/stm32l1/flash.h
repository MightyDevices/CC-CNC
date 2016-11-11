/*
 * flash.h
 *
 *  Created on: 18-06-2012
 *      Author: Tomek
 */

#ifndef STM32L1_FLASH_H_
#define STM32L1_FLASH_H_

#include <stm32l1/stm32l1.h>

/* register base */
#define FLASH_BASE							(0x40023c00)
/* flash controller itself */
#define FLASH								((flash_t *) FLASH_BASE)

typedef struct {
	reg32_t ACR;
	reg32_t PECR;
	reg32_t PDKEYR;
	reg32_t PEKEYR;
	reg32_t PRGKEYR;
	reg32_t OPTKEYR;
	reg32_t SR;
	reg32_t OBR;
	reg32_t WRPR1;
} __attribute__ ((packed)) flash_t;

/* Access control register */
#define FLASH_ACR_LATENCY                   (0x00000001)
#define FLASH_ACR_PRFTEN                    (0x00000002)
#define FLASH_ACR_ACC64                     (0x00000004)
#define FLASH_ACR_SLEEP_PD                  (0x00000008)
#define FLASH_ACR_RUN_PD                    (0x00000010)

/* Program/erase control register */
#define FLASH_PECR_PELOCK                   (0x00000001)
#define FLASH_PECR_PRGLOCK                  (0x00000002)
#define FLASH_PECR_OPTLOCK                  (0x00000004)
#define FLASH_PECR_PROG                     (0x00000008)
#define FLASH_PECR_DATA                     (0x00000010)
#define FLASH_PECR_FTDW                     (0x00000100)
#define FLASH_PECR_ERASE                    (0x00000200)
#define FLASH_PECR_FPRG                     (0x00000400)
#define FLASH_PECR_PARALLBANK               (0x00008000)
#define FLASH_PECR_EOPIE                    (0x00010000)
#define FLASH_PECR_ERRIE                    (0x00020000)
#define FLASH_PECR_OBL_LAUNCH               (0x00040000)

/* Power down key register */
#define FLASH_PDKEYR_PDKEYR                 (0xFFFFFFFF)
/* Program/erase key register */
#define FLASH_PEKEYR_PEKEYR                 (0xFFFFFFFF)
/* Program memory key register */
#define FLASH_PRGKEYR_PRGKEYR               (0xFFFFFFFF)
/* Option byte key register */
#define FLASH_OPTKEYR_OPTKEYR               (0xFFFFFFFF)

/* Status register */
#define FLASH_SR_BSY                        (0x00000001)
#define FLASH_SR_EOP                        (0x00000002)
#define FLASH_SR_ENHV                       (0x00000004)
#define FLASH_SR_READY                      (0x00000008)
#define FLASH_SR_WRPERR                     (0x00000100)
#define FLASH_SR_PGAERR                     (0x00000200)
#define FLASH_SR_SIZERR                     (0x00000400)
#define FLASH_SR_OPTVERR                    (0x00000800)
#define FLASH_SR_OPTVERRUSR                 (0x00001000)

/* Option byte register */
#define FLASH_OBR_RDPRT                     (0x000000AA)
#define FLASH_OBR_BOR_LEV                   (0x0000000F)
#define FLASH_OBR_USER                      (0x00000070)
#define FLASH_OBR_IWDG_SW                   (0x00000010)
#define FLASH_OBR_NRST_STOP                 (0x00000020)
#define FLASH_OBR_NRST_STDBY                (0x00000040)
#define FLASH_OBR_NRST_BFB2                 (0x00000080)

/* Write protection register */
#define FLASH_WRPR_WRP                      (0xFFFFFFFF)
#define FLASH_WRPR1_WRP                     (0xFFFFFFFF)
#define FLASH_WRPR2_WRP                     (0xFFFFFFFF)

/* protecting keys */
#define FLASH_PDKEY1               			(0x04152637)
#define FLASH_PDKEY2               			(0xFAFBFCFD)
#define FLASH_PEKEY1               			(0x89ABCDEF)
#define FLASH_PEKEY2               			(0x02030405)
#define FLASH_PRGKEY1              			(0x8C9DAEBF)
#define FLASH_PRGKEY2              			(0x13141516)
#define FLASH_OPTKEY1              			(0xFBEAD9C8)
#define FLASH_OPTKEY2              			(0x24252627)


#endif /* STM32L1_FLASH_H_ */
