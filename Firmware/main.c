/*
 * main.c
 *
 *      Author: Tomek
 */

#include <stm32l1/rcc.h>
#include <stm32l1/timer.h>
#include <stm32l1/nvic.h>
#include <stm32l1/rtc.h>
#include <stm32l1/flash.h>
#include <stm32l1/uid.h>
#include <cnc/mchn.h>
#include <cnc/motors.h>
#include <dev/watchdog.h>
#include <dev/systime.h>
#include <dev/usart1.h>
#include <dev/power.h>
#include <dev/extimux.h>
#include <dev/cpuclock.h>
#include <dev/usb.h>
#include <dev/usbvcp.h>
#include <dev/led.h>
#include <dev/spi1.h>
#include <dev/sr.h>
#include <dev/drv8824.h>
#include <dev/i2c1.h>
#include <dev/mcp4728.h>
#include <dev/mcp23008.h>
#include <dev/tbase.h>
#include <dev/pwm.h>
#include <dev/tsense.h>
#include <sys/time.h>
#include <sys/err.h>
#include <sys/idle.h>
#include <version.h>
#include <input/input.h>
#include <test/led.h>

#define DEBUG
#include <dev/debug.h>

/* system entry point */
int main(void)
{
	/* TODO initialize watchdog */
//	Watchdog_Init();
	/* kick the dog */
	Watchdog_Kick();

	/* initialize clock */
	CpuClock_Init();
	/* enable clock */
	CpuClock_SetFreq(CPUCLOCK_32M);

	/* initialize systick */
	SysTime_Init();
	/* initial delay */
	SysTime_Delay(500);

	/* initialize power */
	Power_Init();
	/* initialize exti mux */
	ExtiMux_Init();

	/* initialize usart1 for debug */
	USART1_Init();
	/* initial string */
	dprintf("CC-CNC, hw = %s, sw = %s\n", HW_VER, SW_VER);

	/* initialize leds */
//	Led_Init();

	/* initialize usb device */
	USB_Init();
	/* initialize vcp device */
	USBVCP_Init();
	/* reset bus to start enumeration */
	USB_Reset();

	/* enable tool sensor */
	TSense_Init();
	/* initialize pwm output driver */
	Pwm_Init();
	/* initialize spi1 */
	SPI1_Init();
	/* initialize i2c1 */
	I2C1_Init();
	/* initialize time base */
	TBase_Init();

	/* initialize quad dac */
	MCP4728_Init();
	/* initialize gpio expander */
	MCP23008_Init();
	/* initialize shift register */
	SR_Init();
	/* initialize stepper motor driver */
	DRV8824_Init();

	/* motors drive */
	Motors_Init();
	/* stepping controller */
	Mchn_Init();

	/* initialize input parsing */
	Input_Init();

	/* main program loop */
	for (;;) {
		/* poll input */
		Input_Poll();
		/* kick the dog */
		Watchdog_Kick();
	}

	/* never reached */
	return 0;
}
