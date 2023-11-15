
/*
 * hal.h
 *
 *  Created on:
 *      Author: Ultimate
 */
#ifndef HAL_H_
#define HAL_H_
#include <sys/_stdint.h>

typedef enum
{
	LED_OFF,
	LED_ON,
} LED_SWITCH;

typedef enum
{
	IO_LOW,
	IO_HIGTH,
} IO_STATUS;

void axi_write_data(unsigned int addr, int dat);
int axi_read_data(unsigned int addr);

#endif /* HAL_H_ */
