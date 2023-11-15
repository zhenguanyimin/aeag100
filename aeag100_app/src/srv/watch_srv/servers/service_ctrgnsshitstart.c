#include "FreeRTOS.h"
#include "task.h"
#include "service_ctrgnsshitstart.h"
#include "../../../app/process_task/hit_process_task.h"
#include "service_setworkmode.h"
#include "../../../app/alg/alg_init.h"

static uint8_t uWatch_Start_HitFlag = 0;

enum
{
	TURN_OFF,
	TURN_ON,
};

void Set_switchHitState(uint8_t value)
{
	uWatch_Start_HitFlag = value;
}

uint8_t Get_switchHitState()
{
	return uWatch_Start_HitFlag;
}

void Check_WatchStartHit()
{
	if (1 == uWatch_Start_HitFlag)
	{
		uWatch_Start_HitFlag++;
	}
}

uint16_t Service_CtrGnssHitStart_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint16_t len = 1;
	uint8_t count = 0;
	uint16_t controlCmd = 0; // 0:off, 1:on

	if (1 == Get_WorkMode()) // 只有在测试模式下才可以通过命令开启打击
	{
		controlCmd = request_msg->buffer[0];
		if ((controlCmd != TURN_OFF) && (controlCmd != TURN_ON))
		{
			response_msg->buffer[0] = 0; // 0 : 失败     ,      1 : 成功
			return len;
		}

		if (controlCmd == TURN_ON)
		{
			uWatch_Start_HitFlag = 1;
			HitProcessTask_Post(eEVENT_MANUAL_HIT); //
			while (1 == uWatch_Start_HitFlag)
			{
				if (count >= 5)
				{
					break;
				}
				vTaskDelay(100);
				count++;
			}
			if (2 == uWatch_Start_HitFlag)
			{
				response_msg->buffer[0] = 1; // 0 : 失败     ,      1 : 成功
				uWatch_Start_HitFlag = 3;	 // 开始打击状态
				SetAlgorithmStopFlag(1);
			}
			else
			{
				response_msg->buffer[0] = 0; // 0 : 失败     ,      1 : 成功
				uWatch_Start_HitFlag = 0;	 // 关闭打击状态
			}
		}
		else
		{
			HitProcessTask_Post(eEVENT_CLOSE_HIT);
			while (uWatch_Start_HitFlag != 0)
			{
				if (count >= 5)
				{
					break;
				}
				vTaskDelay(100);
				count++;
			}
			if (uWatch_Start_HitFlag == 0)
			{
				response_msg->buffer[0] = 1; // 0 : 失败     ,      1 : 成功
			}
			else
			{
				response_msg->buffer[0] = 0; // 0 : 失败     ,      1 : 成功
			}
		}
	}
	else
	{
		response_msg->buffer[0] = 0; // 0 : 失败     ,      1 : 成功
	}

	return len;
}

const SERVICE_ENTRY Service_CtrGnssHitStart =
	{
		.id = CMD_CTR_GNSS_HIT_START,
		.entry = Service_CtrGnssHitStart_fun,
};
