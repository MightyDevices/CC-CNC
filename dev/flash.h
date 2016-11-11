/*
 * flash.h
 *
 *  Created on: 10-06-2014
 *      Author: Tomek
 */

#ifndef DEV_FLASH_H_
#define DEV_FLASH_H_

/* initialize flash module */
int Flash_Init(void);
/* initialize flash module */
int Flash_Deinit(void);
/* write page */
void Flash_WritePage(void *dst, void *src);


#endif /* FLASH_H_ */
