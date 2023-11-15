#include <string.h>
#include "service_getcalibvalue.h"
#include "service_setcalibvalue.h"

uint16_t Service_GetCalibValue_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint16_t len = 0;
	channel_volrange_t *data = NULL;

	data = Get_AllChanVoltLimit();
	data->chan1 = 1;
	data->chan1lowlimt = 1030;
	data->chan1uplimt = 2010;
	data->chan2 = 2;
	data->chan2lowlimt = 1031;
	data->chan2uplimt = 2011;
	data->chan3 = 3;
	data->chan3lowlimt = 1032;
	data->chan3uplimt = 2012;
	data->chan4 = 4;
	data->chan4lowlimt = 1033;
	data->chan4uplimt = 2013;

	memcpy(response_msg->buffer, (uint8_t *)Get_AllChanVoltLimit(), sizeof(channel_volrange_t));
	len = sizeof(channel_volrange_t);

	return len;
}

const SERVICE_ENTRY Service_GetCalibValue =
	{
		.id = CMD_GET_CALIBVALUE,
		.entry = Service_GetCalibValue_fun,
};
