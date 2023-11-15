/*
 * service_setBoardSN.c
 *
 *  Created on: 2022年11月1日
 *      Author: A22745
 */

#include "service_setBoardSN.h"
#include "../../../srv/flash_nv/flash_nv.h"

#define SN_LENGTH 14

uint16_t Service_SetBoardSN_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len = 0;
	uint8_t retLen = 0;
	int8_t ret = 0;

	len = (request_msg->msg_head.len_hi << 8) | request_msg->msg_head.len_lo;
	if (len != SN_LENGTH)
	{
		retLen = 1;
		response_msg->buffer[0] = 0;
		return retLen;
	}

	ret = FlashNV_Set(FLASH_NV_ID_SN_BOARD, request_msg->buffer, SN_LENGTH);
	if (ret == 0)
	{
		response_msg->buffer[0] = true;
	}
	else
	{
		response_msg->buffer[0] = false;
	}

	retLen = 1;
	return retLen;
}

const SERVICE_ENTRY Service_SetBoardSN =
	{
		.id = CMD_SET_BOARD_SN,
		.entry = Service_SetBoardSN_fun,
};
