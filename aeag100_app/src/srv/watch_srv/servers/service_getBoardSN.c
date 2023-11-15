/*
 * service_getBoardSN.c
 *
 *  Created on: 2022年11月2日
 *      Author: A22745
 */

#include "../../../srv/flash_nv/flash_nv.h"
#include "service_getBoardSN.h"
#include "../../../cfg/aeag_config.h"

#define SN_LENGTH 14

uint16_t Service_GetBoardSN_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len = 0;
	size_t retLent;
	uint8_t ret = 0;
	uint8_t getBoardSN[SN_LENGTH + 1] = {0};
	ret = FlashNV_Get(FLASH_NV_ID_SN_BOARD, getBoardSN, SN_LENGTH, &retLent);
	if (ret == 0)
	{
		response_msg->buffer[0] = 0;
		len = 1;
	}
	else
	{
		memcpy(response_msg->buffer, (uint8_t *)&getBoardSN[0], SN_LENGTH);
		len = SN_LENGTH;
	}
	return len;
}

uint16_t Service_GetSoftwareVersion_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len = strlen(EMBED_SOFTWARE_PS_VERSION_STR);
	memcpy(response_msg->buffer, EMBED_SOFTWARE_PS_VERSION_STR, len);
	return len;
}

const SERVICE_ENTRY Service_GetBoardSN =
	{
		.id = CMD_GET_BOARD_SN,
		.entry = Service_GetBoardSN_fun,
};

const SERVICE_ENTRY Service_GetSoftwareVersion =
	{
		.id = CMD_GET_SOFTWARE_VERSION,
		.entry = Service_GetSoftwareVersion_fun,
};
