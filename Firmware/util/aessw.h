/*
 * aes.h
 *
 *  Created on: 09-10-2013
 *      Author: Tomek
 */

#ifndef UTIL_AES_H_
#define UTIL_AES_H_

#include <stdint.h>

/* generate round keys: ekey will hold 11 keys (176 bytes) */
void AESSW_KeySchedule(uint8_t *key, uint8_t *ekey);
/* perform encryption */
void AESSW_Encrypt(uint8_t *ekey, uint8_t *state);
/* perform decryption */
void AESSW_Decrypt(uint8_t *ekey, uint8_t *state);


#endif /* AES_H_ */
