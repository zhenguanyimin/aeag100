#include "service_manager.h"

uint16_t Service_UpdateLib_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	return 0;
}

const SERVICE_ENTRY Service_UpdateLib =
	{
		.id = CMD_UPDATE_LIB,
		.entry = Service_UpdateLib_fun,
};
