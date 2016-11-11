/*
 * mcp4728.c
 *
 *  Created on: Aug 28, 2016
 *      Author: Tomek
 */

#include <sys/err.h>
#include <dev/i2c1.h>

#define DEBUG
#include <dev/debug.h>

/* general call address */
#define MCP4728_GEN_ADDR					0x00
/* device i2c address */
#define MCP4728_ADDR						0x60

/* general call commands */
#define MCP4728_GC_RESET					0x06
#define MCP4728_GC_WAKEUP					0x09
#define MCP4728_GC_UPDATE					0x08

/* transfer callback */
static cb_t callback;
/* transfer buffer */
static uint8_t buf[12];

/* i2c operation callback */
static void MCP4728_ChanelsUpdatedCallback(void *arg)
{
	i2c1_cbarg_t *ea = arg;
	/* release semaphore */
	Sem_Release(&i2c1_sem);

	/* call callback if not empty or null */
	if (callback != CB_NULL) {
		callback((void *)&ea->error);
	/* change the address to notify sync caller that operation has ended */
	} else {
		callback = CB_NONE;
	}
}

/* initialize quad output dac */
int MCP4728_Init(void)
{
	uint16_t x = 0;
	/* lock resource */
	Sem_Lock(&i2c1_sem);
	/* perform test transfer */
	int rc = I2C1_Transfer(I2C1_WRITE, MCP4728_ADDR, 0, &x, 1, 0);
	/* release resource */
	Sem_Release(&i2c1_sem);

	/* check status */
	if (rc >= EOK)
		rc = EOK;

	/* some debug maybe? */
	dprintf("rc = %d\n", rc);

	/* report status */
	return rc;
}

/* set four channel values */
int MCP4728_SetChannels(uint16_t ch[4], cb_t cb)
{
	/* result code, sync or async call? */
	int rc, sync = cb == CB_NULL;

	/* lock the resource */
	rc = sync ? Sem_Lock(&i2c1_sem) : Sem_TryLock(&i2c1_sem);
	/* unable to lock semaphore? */
	if (rc != EOK)
		return rc;

	/* store callback */
	callback = cb;

	/* command word, channel words... */
	buf[0] = 0x40; buf[1]  = 0x80 | ((ch[0] >> 8) & 0xf); buf[2]  = ch[0];
	buf[3] = 0x02; buf[4]  = 0x80 | ((ch[1] >> 8) & 0xf); buf[5]  = ch[1];
	buf[6] = 0x04; buf[7]  = 0x80 | ((ch[2] >> 8) & 0xf); buf[8]  = ch[2];
	buf[9] = 0x06; buf[10] = 0x80 | ((ch[3] >> 8) & 0xf); buf[11] = ch[3];

	/* start xmission */
	rc = I2C1_Transfer(I2C1_WRITE, MCP4728_ADDR, buf[0], buf + 1, 11,
			MCP4728_ChanelsUpdatedCallback);

	/* synchronous operation */
	while (sync && callback == CB_NULL);
	/* report status */
	return rc;
}
