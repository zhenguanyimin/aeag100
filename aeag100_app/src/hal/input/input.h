/*
 * input.h
 *
 *  Created on:
 *      Author: Ultimate
 */
#ifndef INPUT_H_
#define INPUT_H_
#include <sys/_stdint.h>

#include "../hal.h"
typedef enum
{
	IO_ROTATE_SIA,
	IO_ROTATE_SIB,
	IO_ROTATE_SW,
	IO_GPS_KEY = 0x6A,
	IO_KEY2_SET,
	IO_KEY3,
	IO_KEY4,
	IO_KEY1_HIT = 0x63,
	INPUT_NAME_MAX
}INPUT_NAME;

uint8_t Input_GetValue(INPUT_NAME name);

#endif /* INPUT_H_ */
