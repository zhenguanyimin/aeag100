#include "service_getswitchstate.h"

uint16_t Service_GetSwitchState_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t switchstate = 0;
	uint16_t len = 0;

	switchstate = Get_switchHitState();
	if (switchstate == 3)
	{
		response_msg->buffer[0] = 1;
	}
	else
	{
		response_msg->buffer[0] = 0;
	}
	len = 1;

	return len;
}

const SERVICE_ENTRY Service_GetSwitchState =
	{
		.id = CMD_GET_SWITCHSTATE,
		.entry = Service_GetSwitchState_fun,
};
