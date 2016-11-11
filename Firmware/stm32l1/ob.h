/*
 * ob.h
 *
 *  Created on: 23-07-2015
 *      Author: Tomek
 */

#ifndef STM32L1_OB_H_
#define STM32L1_OB_H_

/* register base */
#define OB_BASE             				(0x1FF80000)
/* option bytes */
#define OB                  				((ob_t *) OB_BASE)

/* option bytes */
typedef struct {
  reg32_t RDP;
  reg32_t USER;
  reg32_t WRP01;
  reg32_t WRP23;
  reg32_t WRP45;
  reg32_t WRP67;
  reg32_t WRP89;
  reg32_t WRP1011;
} __attribute__ ((packed)) ob_t;

#endif /* STM32L1_OB_H_ */
