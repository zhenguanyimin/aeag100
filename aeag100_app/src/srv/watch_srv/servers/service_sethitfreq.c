#include "service_sethitfreq.h"
#include "../../../app/key_handle/key_handle.h"

uint16_t Service_SetHitFreq_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t freqmode = 0;
	uint16_t len = 0;

	freqmode = request_msg->buffer[0];
	SetStrikeFreqItem(freqmode);		   // 设置打击频率
	StrikeFreqOutputCtrl(freqmode, eSHOW); // 显示打击频率
	if (freqmode == GetStrikeFreqItem())
	{
		response_msg->buffer[0] = 1; // 0 : 失败     , 1 : 成功
	}
	else
	{
		response_msg->buffer[0] = 0; // 0 : 失败     , 1 : 成功
	}
	len = 1;

	return len;
}

const SERVICE_ENTRY Service_SetHitFreq =
	{
		.id = CMD_SET_HIT_FREQ,
		.entry = Service_SetHitFreq_fun,
};
