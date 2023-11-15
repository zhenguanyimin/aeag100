#ifndef SERVICE_SETCALIBVALUE_H
#define SERVICE_SETCALIBVALUE_H

#include "service_manager.h"

typedef struct channel_volrange
{
	uint16_t chan1; // A通道  5.2GHz
	uint16_t chan1lowlimt;
	uint16_t chan1uplimt;
	uint16_t chan2; // A通道  5.8GHz
	uint16_t chan2lowlimt;
	uint16_t chan2uplimt;
	uint16_t chan3; // B通道  2.4GHz
	uint16_t chan3lowlimt;
	uint16_t chan3uplimt;
	uint16_t chan4; // B通道  5.8GHz
	uint16_t chan4lowlimt;
	uint16_t chan4uplimt;
} channel_volrange_t;

channel_volrange_t *Get_AllChanVoltLimit();
uint16_t Service_SetCalibValue_fun(alink_msg_t *request_msg, alink_msg_t *response_msg);

#endif
