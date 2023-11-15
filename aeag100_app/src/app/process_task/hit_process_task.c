/*
 * hit_process_task.c
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hit_process_task.h"
#include <timers.h>

#include "../../cfg/aeag_config.h"
#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../hal/input/input.h"
#include "../../hal/output/output.h"
#include "../../srv/log/log.h"
#include "../../srv/watch_srv/servers/service_ctrgnsshitstart.h"
#include "../alg/alg_init.h"
#include "../DataPath/data_path.h"
#include "../key_handle/key_handle.h"
#include "../rf_config/rf_config.h"
#include "../logging/logging.h"
#include "../switch_filter/switch_filter.h"

static QueueHandle_t xHitProcessQueue;
static TimerHandle_t TimerHit_Handle = NULL;
static TimerHandle_t TimerWarning_Handle = NULL;
static uint8_t uAeagHitFlag = 0;
static uint8_t uWarningLeval = 0;
static uint32_t uWarningcnt = 0;
static uint8_t uSysWorkingMode = 0;
static sLogging_msg_t msg;
void SetSysWorkingMode(uint8_t mode)
{
	uSysWorkingMode = mode;
}

uint8_t GetSysWorkingMode(void)
{
	return uSysWorkingMode;
}

void HitProcessTask_Post(uint32_t flag)
{
	if (xHitProcessQueue != NULL)
	{
		xQueueSend(xHitProcessQueue, &flag, 10);
	}
}

static int32_t HitProcessTask_Pend(uint32_t *bufaddr, uint32_t timeout)
{

	int32_t result = -1;

	if (xHitProcessQueue != NULL)
	{
		if (xQueueReceive(xHitProcessQueue, bufaddr, timeout))
		{
			result = 0;
		}
	}

	return result;
}

static int32_t frequency_register_setting(uint8_t freItem)
{
	int32_t result = 0;
	enum FREQ_SELE localfreItem = (enum FREQ_SELE)freItem;

	switch (localfreItem)
	{
	case eFREQ_SELE_ALL:
	{
		break;
	}
	case eFREQ_SELE_2G:
	{
		break;
	}
	case eFREQ_SELE_2_4G:
	{
		break;
	}
	case eFREQ_SELE_4_6G:
	{
		break;
	}
	case eFREQ_SELE_2G_2_4G:
	{
		break;
	}
	case eFREQ_SELE_2_4G_4_6G:
	{
		break;
	}
	case eFREQ_SELE_2G_4_6G:
	{
		break;
	}
	default:
	{
		result = -1;
		break;
	}
	}

	return result;
}

void HitProcess_Task(void *p_arg)
{
	uint32_t flag;
	uint8_t uStrikeFreqItem;
	uint8_t uGps915mHitFlag = 0;

	while (1)
	{
		if (HitProcessTask_Pend(&flag, 500) == 0)
		{
			enum HIT_EVENT hit_flag = (enum HIT_EVENT)flag;
			switch (hit_flag)
			{
			case eEVENT_AUTO_HIT:
			{
				break;
			}
			case eEVENT_MANUAL_HIT:
			{
				SetSysWorkingMode(1);
				uAeagHitFlag = 1;
				SimulaAdp_SendAdrf5250md(eRF5250_CLOSE);
				uStrikeFreqItem = GetStrikeFreqItem();
				StrikeFreqOutputCtrl(uStrikeFreqItem, eSHOW_STRIKE);
				xTimerStart(TimerHit_Handle, 10);
				GPIO_OutputCtrl(IO_DAC_PD, IO_LOW);
				GPIO_OutputCtrl(IO_VBRATOR, IO_HIGTH);
				xTimerStop(TimerWarning_Handle, 10);
				GPIO_OutputCtrl(IO_LED_WARNING, IO_LOW);
				GPIO_OutputCtrl(IO_BUZZ, IO_LOW);

				if (Input_GetValue(IO_GPS_KEY) == 0)
				{
					uGps915mHitFlag += Freq_TX_GPS;
				}
				if ((uStrikeFreqItem == eFREQ_SELE_ALL) || (uStrikeFreqItem == eFREQ_SELE_2G) || (uStrikeFreqItem == eFREQ_SELE_2G_2_4G) || (uStrikeFreqItem == eFREQ_SELE_2G_4_6G))
				{
					uGps915mHitFlag += Freq_TX_915m;
				}

				if (uGps915mHitFlag == Freq_TX_915m)
				{
					GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_HIGTH);
					Rf_ConfigTask_Post(eRF_AD9361_CFG_HIT_915);
				}
				else if (uGps915mHitFlag == Freq_TX_GPS)
				{
					GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_HIGTH);
					Rf_ConfigTask_Post(eRF_AD9361_CFG_HIT_GPS);
				}
				else if (uGps915mHitFlag == Freq_TX_GPS_915m)
				{
					GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_HIGTH);
					Rf_ConfigTask_Post(eRF_AD9361_CFG_HIT_GPS_915);
				}

				msg.uLogNum = eLOG_START_HIT_FRE;
				msg.StartHit_msg.uTimestamp = (uint32_t)xTaskGetTickCount();
				msg.StartHit_msg.uFreItem = uStrikeFreqItem;
				msg.StartHit_msg.uGpsOnFlag = (uGps915mHitFlag & 0x02) >> 1;
				LoggingTask_Post(&msg);
				Check_WatchStartHit();
				frequency_register_setting(uStrikeFreqItem);
				break;
			}
			case eEVENT_CLOSE_HIT:
			{
				SetSysWorkingMode(0);
				GPIO_OutputCtrl(IO_DAC_PD, IO_HIGTH);
				StrikeFreqOutputCtrl(0xFF, eSHOW_STRIKE);
				GPIO_OutputCtrl(IO_VBRATOR, IO_LOW);
				xTimerStop(TimerHit_Handle, 10);
				if (uAeagHitFlag)
				{
					msg.uLogNum = eLOG_END_HIT;
					msg.EndHit_msg.uTimestamp = (uint32_t)xTaskGetTickCount();
					LoggingTask_Post(&msg);
					//					SimulaAdp_SendAdrf5250md(eRF5250_CHANNEL3);
				}
				SimulaAdp_SendAdrf5250md(eRF5250_CHANNEL3);
				if (0 != uGps915mHitFlag) //
				{
					uGps915mHitFlag = 0;
					Rf_ConfigTask_Post(eRF_AD9361_CFG_CLOSE_HIT);
					//					reset_ad936x();
				}
				else
				{
					SetAlgorithmStopFlag(0);
					axi_write_data(0x01, 1);
					data_path_out_en(4);
				}
				Set_switchHitState(0x0);
				//				tinyLog_refresh();
				uAeagHitFlag = 0;
				break;
			}
			case eEVENT_GNSS_HIT:
			{
				break;
			}
			case eEVENT_CLOSE_GNSS_HIT:
			{
				break;
			}
			case eEVENT_NO_WARNING_AND_REMAIN:
			{
				LOG_DEBUG("eEVENT_NO_WARNING  NO\r\n");
				xTimerStop(TimerWarning_Handle, 10);
				GPIO_OutputCtrl(IO_LED_WARNING, IO_LOW);
				GPIO_OutputCtrl(IO_BUZZ, IO_LOW);
				break;
			}
			case eEVENT_WARNING_LEVAL1:
			{
				LOG_DEBUG("eEVENT_WARNING_LEVAL1\r\n");
				uWarningLeval = 1;
				xTimerStart(TimerWarning_Handle, 10);
				break;
			}
			case eEVENT_WARNING_LEVAL2:
			{
				LOG_DEBUG("eEVENT_WARNING_LEVAL2\r\n");
				uWarningLeval = 2;
				xTimerStart(TimerWarning_Handle, 10);
				break;
			}
			case eEVENT_WARNING_LEVAL3:
			{
				LOG_DEBUG("eEVENT_WARNING_LEVAL3\r\n");
				uWarningLeval = 3;
				xTimerStart(TimerWarning_Handle, 10);
				break;
			}
			case eEVENT_NO_WARNING:
			{
				LOG_DEBUG("eEVENT_NO_WARNING \r\n");
				uWarningLeval = 0;
				xTimerStop(TimerWarning_Handle, 10);
				GPIO_OutputCtrl(IO_LED_WARNING, IO_LOW);
				GPIO_OutputCtrl(IO_BUZZ, IO_LOW);
				break;
			}
			default:
				break;
			}
		}
	}
}

static void TimerHit_Callback(void)
{
	static uint8_t cnt2 = 0;
	cnt2++;
}

static void TimerWarning_Callback(void)
{
	if (uWarningLeval == 1)
	{
		if (uWarningcnt % 5 == 0)
		{
			GPIO_OutputCtrl(IO_LED_WARNING, LED_ON);
			GPIO_OutputCtrl(IO_BUZZ, IO_HIGTH);
		}
		else
		{
			GPIO_OutputCtrl(IO_LED_WARNING, LED_OFF);
			GPIO_OutputCtrl(IO_BUZZ, IO_LOW);
		}
	}
	else if (uWarningLeval == 2)
	{
		if ((uWarningcnt % 2 == 0) && (uWarningcnt % 4 != 0))
		{
			GPIO_OutputCtrl(IO_LED_WARNING, LED_ON);
			GPIO_OutputCtrl(IO_BUZZ, IO_HIGTH);
		}
		else
		{
			GPIO_OutputCtrl(IO_LED_WARNING, LED_OFF);
			GPIO_OutputCtrl(IO_BUZZ, IO_LOW);
		}
	}
	else if (uWarningLeval == 3)
	{
		if (uWarningcnt % 2 == 0)
		{
			GPIO_OutputCtrl(IO_LED_WARNING, LED_ON);
			GPIO_OutputCtrl(IO_BUZZ, IO_HIGTH);
		}
		else
		{
			GPIO_OutputCtrl(IO_LED_WARNING, LED_OFF);
			GPIO_OutputCtrl(IO_BUZZ, IO_LOW);
		}
	}

	//	GPIO_OutputCtrl(IO_LED_WARNING, (cnt % 2));
	//	GPIO_OutputCtrl(IO_BUZZ, (cnt % 2));
	uWarningcnt++;
	//	if (cnt > 3)
	//	{
	//		cnt = 1;
	//	}
}

int32_t InitHitProcess_Task()
{
	int32_t retVal = -1;
	int32_t *pvTimerID = (int32_t *)1;

	TimerHit_Handle = xTimerCreate((const char *)"Timer",
								   (TickType_t)20,
								   (UBaseType_t)pdTRUE,
								   (void *)pvTimerID,
								   (TimerCallbackFunction_t)TimerHit_Callback);
	if (TimerHit_Handle == NULL)
	{
		return retVal;
	}

	TimerWarning_Handle = xTimerCreate((const char *)"Timer",
									   (TickType_t)200,
									   (UBaseType_t)pdTRUE,
									   (void *)pvTimerID,
									   (TimerCallbackFunction_t)TimerWarning_Callback);
	if (TimerWarning_Handle == NULL)
	{
		return retVal;
	}

	xHitProcessQueue = xQueueCreate(50, sizeof(uint32_t));

	if (xTaskCreate(HitProcess_Task, "HitProcess_Task", TASK_STACK_SIZE_HITPROCESS, NULL, TASK_PRI_HITPROCESS, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
