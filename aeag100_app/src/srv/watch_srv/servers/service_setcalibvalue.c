#include "service_setcalibvalue.h"
#include "../../alink/alink_msg_proc.h"

static channel_volrange_t uChanVoltRange = {0};

channel_volrange_t *Get_AllChanVoltLimit()
{
	return &uChanVoltRange;
}

uint16_t Service_SetCalibValue_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint16_t chanID = 0;
	uint16_t voltlowlimit = 0.f;
	uint16_t voltuplimit = 0.f;
	uint16_t len = 0;

	chanID = request_msg->buffer[0] + request_msg->buffer[1] << 8;
	voltlowlimit = request_msg->buffer[2] + request_msg->buffer[3] << 8;
	voltuplimit = request_msg->buffer[4] + request_msg->buffer[5] << 8;
	switch (chanID)
	{
	case 1:
		uChanVoltRange.chan1 = 1;
		uChanVoltRange.chan1lowlimt = voltlowlimit;
		uChanVoltRange.chan1uplimt = voltuplimit;
		response_msg->buffer[0] = 1;
		break;
	case 2:
		uChanVoltRange.chan2 = 1;
		uChanVoltRange.chan2lowlimt = voltlowlimit;
		uChanVoltRange.chan2uplimt = voltuplimit;
		response_msg->buffer[0] = 1;
		break;
	case 3:
		uChanVoltRange.chan3 = 1;
		uChanVoltRange.chan3lowlimt = voltlowlimit;
		uChanVoltRange.chan3uplimt = voltuplimit;
		response_msg->buffer[0] = 1;
		break;
	case 4:
		uChanVoltRange.chan4 = 1;
		uChanVoltRange.chan4lowlimt = voltlowlimit;
		uChanVoltRange.chan4uplimt = voltuplimit;
		response_msg->buffer[0] = 1;
		break;
	default:
		response_msg->buffer[0] = 0;
		break;
	}
	len = 1;

	return len;
}

const SERVICE_ENTRY Service_SetCalibValue =
	{
		.id = CMD_SET_CALIBVALUE,
		.entry = Service_SetCalibValue_fun,
};
