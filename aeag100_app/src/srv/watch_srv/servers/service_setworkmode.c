#include "service_setworkmode.h"

static uint8_t uWorkMode = 0;

uint8_t Get_WorkMode()
{
	return uWorkMode;
}

uint16_t Service_SetWorkMode_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint16_t len = 0;

	if ((0 == request_msg->buffer[0]) || (1 == request_msg->buffer[0]) || (2 == request_msg->buffer[0]))
	{
		response_msg->buffer[0] = 0x1;
		uWorkMode = request_msg->buffer[0];
	}
	else
	{
		response_msg->buffer[0] = 0x0; // 模式设置失败
	}
	len = 1;

	return len;
}

const SERVICE_ENTRY Service_SetWorkMode =
	{
		.id = CMD_SET_WORKMODE,
		.entry = Service_SetWorkMode_fun,
};
