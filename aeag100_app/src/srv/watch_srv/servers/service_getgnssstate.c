/*
 * service_getgnssstate.c
 *
 *  Created on: 2022年11月16日
 *      Author: A22745
 */
#include "service_getgnssstate.h"
#include "../../../hal/input/input.h"
enum
{
	TURN_OFF,
	TURN_ON,
};

uint16_t Service_GetGNSSState_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len = 0;

	if (Input_GetValue(IO_GPS_KEY) == 0)
	{
		response_msg->buffer[0] = TURN_ON;
	}
	else
	{
		response_msg->buffer[0] = TURN_OFF;
	}
	len = 1;

	return len;
}

const SERVICE_ENTRY Service_GetGNSSState =
	{
		.id = CMD_GET_GNSS_STATE,
		.entry = Service_GetGNSSState_fun,
};
