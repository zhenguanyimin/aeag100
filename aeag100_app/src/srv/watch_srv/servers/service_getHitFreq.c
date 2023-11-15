/*
 * service_getHitFreq.c
 *
 *  Created on: 2022年11月17日
 *      Author: A22745
 */
#include "service_getHitFreq.h"
#include "../../../app/key_handle/key_handle.h"

uint16_t Service_GetHitFreq_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len = 0;
	response_msg->buffer[0] = GetStrikeFreqItem();
	len = 1;

	return len;
}

const SERVICE_ENTRY Service_GetHitFreq =
	{
		.id = CMD_GET_HIT_FREQ,
		.entry = Service_GetHitFreq_fun,
};
