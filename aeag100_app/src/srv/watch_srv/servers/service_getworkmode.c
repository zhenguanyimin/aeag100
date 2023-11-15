#include "service_getworkmode.h"
#include "service_setworkmode.h"

uint16_t Service_GetWorkMode_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len = 0;

	response_msg->buffer[0] = Get_WorkMode();
	len = 1;

	return len;
}

const SERVICE_ENTRY Service_GetWorkMode =
	{
		.id = CMD_GET_WORKMODE,
		.entry = Service_GetWorkMode_fun,
};
