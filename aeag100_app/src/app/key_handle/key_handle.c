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
#include <timers.h>

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xil_printf.h>
#include "key_handle.h"
#include "../../cfg/aeag_config.h"
#include "../../hal/input/input.h"
#include "../../hal/output/output.h"
#include "../../srv/log/log.h"
#include "../process_task/hit_process_task.h"
#include "../../test/ut/ut_reg_set.h"
#include "../alg/alg_init.h"
#include "../DataPath/data_path.h"
#include <timers.h>
#include "../battery/battery_info.h"
#include "../../hal/smbus_driver/smbus_driver.h"
#include "../logging/logging.h"
#include "../process_task/detect_process_task.h"

#define PERIOD_3_S 30
#define BATTERY_CAP_WARNING_THRESHOLD 10
static sLogging_msg_t msg;
static QueueHandle_t xKeyHandleQueue;
static TimerHandle_t TimerKeyHandle = NULL;
static TimerHandle_t TimerRotateHandle = NULL;
static TimerHandle_t TimerHitTimeout_Handle = NULL;
static uint8_t RotateStopRes = 0;
static uint8_t uHitStatus = 0;
static bool isGetBatteryStateFlag = true;
static uint8_t uHitKeyRelease = IO_HIGTH;

void KeyHandleTask_Post(uint32_t flag)
{
	int32_t xHigherPriorityTaskWoken;
	if (xKeyHandleQueue != NULL)
	{
		if (xQueueIsQueueFullFromISR(xKeyHandleQueue) == 0)
		{
			xQueueSendFromISR(xKeyHandleQueue, &flag, &xHigherPriorityTaskWoken);
		}
	}
}

uint8_t GetHitStatus(void)
{
	return uHitStatus;
}

static uint8_t uStrikeFreqItem = eFREQ_SELE_ALL;
void SetStrikeFreqItem(uint8_t value)
{
	if (value < eFREQ_SELE_MAX)
	{
		uStrikeFreqItem = value;
	}
	else
	{
		uStrikeFreqItem = 0;
	}
}

uint8_t GetStrikeFreqItem(void)
{
	return uStrikeFreqItem;
}

void StrikeFreqOutputCtrl(uint8_t StrikeFreq, uint8_t mode)
{
	enum FREQ_SELE localStrikeFreq = (enum FREQ_SELE)StrikeFreq;

	switch (localStrikeFreq)
	{
	case eFREQ_SELE_ALL:
	{
		if ((mode & 0x01) == eSHOW)
		{
			GPIO_OutputCtrl(IO_LED_ALL, LED_ON);
			GPIO_OutputCtrl(IO_LED_2G, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2_4G, LED_OFF);
			GPIO_OutputCtrl(IO_LED_4_6G, LED_OFF);
		}
		if ((mode & 0x02) == eSTRIKE)
		{
			GPIO_OutputCtrl(IO_BOOST_ON_OFF_28V, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_C_SW_2_4G, IO_HIGTH);
			vTaskDelay(pdMS_TO_TICKS(10));
			GPIO_OutputCtrl(IO_FPGA_S_SW_4_6G, IO_HIGTH);
			vTaskDelay(pdMS_TO_TICKS(10));
			GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_HIGTH);
		}
	}
	break;
	case eFREQ_SELE_2G:
	{
		if ((mode & 0x01) == eSHOW)
		{
			GPIO_OutputCtrl(IO_LED_ALL, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2G, LED_ON);
			GPIO_OutputCtrl(IO_LED_2_4G, LED_OFF);
			GPIO_OutputCtrl(IO_LED_4_6G, LED_OFF);
		}
		if ((mode & 0x02) == eSTRIKE)
		{
			GPIO_OutputCtrl(IO_BOOST_ON_OFF_28V, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_C_SW_2_4G, IO_LOW);
			GPIO_OutputCtrl(IO_FPGA_S_SW_4_6G, IO_LOW);
			GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_HIGTH);
		}
	}
	break;
	case eFREQ_SELE_2_4G:
	{
		if ((mode & 0x01) == eSHOW)
		{
			GPIO_OutputCtrl(IO_LED_ALL, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2G, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2_4G, LED_ON);
			GPIO_OutputCtrl(IO_LED_4_6G, LED_OFF);
		}
		if ((mode & 0x02) == eSTRIKE)
		{
			GPIO_OutputCtrl(IO_BOOST_ON_OFF_28V, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_C_SW_2_4G, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_S_SW_4_6G, IO_LOW);
			GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_LOW);
		}
	}
	break;
	case eFREQ_SELE_4_6G:
	{
		if ((mode & 0x01) == eSHOW)
		{
			GPIO_OutputCtrl(IO_LED_ALL, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2G, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2_4G, LED_OFF);
			GPIO_OutputCtrl(IO_LED_4_6G, LED_ON);
		}
		if ((mode & 0x02) == eSTRIKE)
		{
			GPIO_OutputCtrl(IO_BOOST_ON_OFF_28V, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_C_SW_2_4G, IO_LOW);
			GPIO_OutputCtrl(IO_FPGA_S_SW_4_6G, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_LOW);
		}
	}
	break;
	case eFREQ_SELE_2G_2_4G:
	{
		if ((mode & 0x01) == eSHOW)
		{
			GPIO_OutputCtrl(IO_LED_ALL, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2G, LED_ON);
			GPIO_OutputCtrl(IO_LED_2_4G, LED_ON);
			GPIO_OutputCtrl(IO_LED_4_6G, LED_OFF);
		}
		if ((mode & 0x02) == eSTRIKE)
		{
			GPIO_OutputCtrl(IO_BOOST_ON_OFF_28V, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_C_SW_2_4G, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_S_SW_4_6G, IO_LOW);
			vTaskDelay(pdMS_TO_TICKS(10));
			GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_HIGTH);
		}
	}
	break;
	case eFREQ_SELE_2_4G_4_6G:
	{
		if ((mode & 0x01) == eSHOW)
		{
			GPIO_OutputCtrl(IO_LED_ALL, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2G, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2_4G, LED_ON);
			GPIO_OutputCtrl(IO_LED_4_6G, LED_ON);
		}
		if ((mode & 0x02) == eSTRIKE)
		{
			GPIO_OutputCtrl(IO_BOOST_ON_OFF_28V, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_C_SW_2_4G, IO_HIGTH);
			vTaskDelay(pdMS_TO_TICKS(10));
			GPIO_OutputCtrl(IO_FPGA_S_SW_4_6G, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_LOW);
		}
	}
	break;
	case eFREQ_SELE_2G_4_6G:
	{
		if ((mode & 0x01) == eSHOW)
		{
			GPIO_OutputCtrl(IO_LED_ALL, LED_OFF);
			GPIO_OutputCtrl(IO_LED_2G, LED_ON);
			GPIO_OutputCtrl(IO_LED_2_4G, LED_OFF);
			GPIO_OutputCtrl(IO_LED_4_6G, LED_ON);
		}
		if ((mode & 0x02) == eSTRIKE)
		{
			GPIO_OutputCtrl(IO_BOOST_ON_OFF_28V, IO_HIGTH);
			GPIO_OutputCtrl(IO_FPGA_C_SW_2_4G, IO_LOW);
			GPIO_OutputCtrl(IO_FPGA_S_SW_4_6G, IO_HIGTH);
			vTaskDelay(pdMS_TO_TICKS(10));
			GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_HIGTH);
		}
	}
	break;
	case eFREQ_CLOSE_HIT:
	{
		if ((mode & 0x02) == eSTRIKE)
		{
			GPIO_OutputCtrl(IO_BOOST_ON_OFF_28V, IO_LOW);
			GPIO_OutputCtrl(IO_FPGA_C_SW_2_4G, IO_LOW);
			GPIO_OutputCtrl(IO_FPGA_S_SW_4_6G, IO_LOW);
			GPIO_OutputCtrl(IO_FPGA_SUB_SW_2G, IO_LOW);
		}
	}
	break;
	default:
		break;
	}
}

static int32_t KeyHandleTask_Pend(uint32_t *flag, uint32_t timeout)
{
	int32_t result = -1;

	if (xKeyHandleQueue != NULL)
	{
		if (xQueueReceive(xKeyHandleQueue, flag, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void KeyHandle_Task(void *p_arg)
{
	uint32_t flag;
	uint8_t uStrikeFreqItemInTask;
	//	uint8_t led_on = 0;
	//	uint8_t uGpsKeyReleasePressStatus = 0;
	uint8_t uHitKeyReleasePressStatus = 0;
	uint8_t remaincap = 0;
	uint16_t timeCountPer100ms = 1;

	uStrikeFreqItem = GetStrikeFreqItem();
	StrikeFreqOutputCtrl(uStrikeFreqItem, eSHOW);
	GPIO_OutputCtrl(IO_LED_BAT_GREEN, LED_ON);

	if (Input_GetValue(IO_KEY1_HIT) == IO_LOW)
	{
		uHitKeyRelease = IO_LOW;
		//		data_path_out_en(0);
		//		SetAlgorithmStopFlag(1);
	}
	smbus_sw_init();
	remaincap = UpdataBatteryCapacityAndDisplayLedState(remaincap);
	while (1)
	{
		if (KeyHandleTask_Pend(&flag, 100) == 0)
		{
			enum KEY_TASK eKEY_flag = (enum KEY_TASK)flag;
			switch (eKEY_flag)
			{
			case eKEY_TASK_HIT:
			{
				// todo 考虑消抖处理
				//				HitProcessTask_Post(eEVENT_MANUAL_HIT);
				LOG_DEBUG(" eKEY_TASK_HIT KEY4_intr_Handler \n\r");
				break;
			}
			case eKEY_TASK_SET:
			{
				// todo 考虑消抖处理
				LOG_DEBUG(" eKEY_TASK_SET \n\r");
				break;
			}
			case eKEY_TASK_ROTATE_CLOCKWISE:
			{
				if (RotateStopRes == 0)
				{
					RotateStopRes = 1;
					xTimerStart(TimerRotateHandle, 10);
					uStrikeFreqItemInTask = GetStrikeFreqItem();
					uStrikeFreqItemInTask++;
					if (uStrikeFreqItemInTask >= eFREQ_SELE_MAX)
					{
						uStrikeFreqItemInTask = 0;
					}
					SetStrikeFreqItem(uStrikeFreqItemInTask);
					StrikeFreqOutputCtrl(uStrikeFreqItemInTask, eSHOW);
				}
				break;
			}
			case eKEY_TASK_ROTATE_REVERSE:
			{
				if (RotateStopRes == 0)
				{
					RotateStopRes = 1;
					xTimerStart(TimerRotateHandle, 10);
					uStrikeFreqItemInTask = GetStrikeFreqItem();
					if (uStrikeFreqItemInTask == 0)
					{
						uStrikeFreqItemInTask = eFREQ_SELE_MAX;
					}
					SetStrikeFreqItem(--uStrikeFreqItemInTask);
					StrikeFreqOutputCtrl(uStrikeFreqItemInTask, eSHOW);
				}
				break;
			}
			case eKEY_TASK_ROTATE_SW:
			{
				break;
			}
			default:
				break;
			}
		}
		GPIO_OutputCtrl(IO_LED_GNSS, !Input_GetValue(IO_GPS_KEY)); // GNSS LED灯点亮

		if (Input_GetValue(IO_KEY1_HIT) == uHitKeyRelease)
		{
			if ((uHitStatus > 0)) // 防止按键没有被释放
			{
				if (uHitStatus == 2)
				{
					ClearUAVWarnSlideWinInfo();
					RotateStopRes = 0;
					msg.uLogNum = eLOG_KEY_TRIGGER;
					msg.KeyTrigger_msg.uTimestamp = (uint32_t)xTaskGetTickCount();
					msg.KeyTrigger_msg.IO_Status = 1;
					LoggingTask_Post(&msg);
				}
				uHitStatus--;
				LOG_DEBUG("eEVENT_CLOSE_HIT\r\n");
				xTimerStop(TimerKeyHandle, 10);
				isGetBatteryStateFlag = true;
				HitProcessTask_Post(eEVENT_CLOSE_HIT);
				xTimerStop(TimerHitTimeout_Handle, 10);
			}
			uHitKeyReleasePressStatus = 0;
		}
		else
		{
			if (uHitStatus == 0)
			{
				uHitStatus = 3;
			}

			if (uHitKeyReleasePressStatus == 0)
			{
				data_path_out_en(0);
				SetAlgorithmStopFlag(1);
				axi_write_data(0x01, 0);
				uHitKeyReleasePressStatus = 1;
				if ((remaincap < BATTERY_CAP_WARNING_THRESHOLD) && (remaincap > 0))
				{
					xTimerStart(TimerKeyHandle, 10);
					isGetBatteryStateFlag = false;
					HitProcessTask_Post(eEVENT_NO_WARNING_AND_REMAIN);
					LOG_DEBUG("hit key action eEVENT_NO_WARNING_AND_REMAIN,remaincp=%d\r\n", remaincap);
				}
				else
				{
					RotateStopRes = 1;
					HitProcessTask_Post(eEVENT_MANUAL_HIT);
					xTimerStop(TimerKeyHandle, 10);
					//					xTimerStart(TimerHitTimeout_Handle, 10);
					isGetBatteryStateFlag = true;
				}
				msg.uLogNum = eLOG_KEY_TRIGGER;
				msg.KeyTrigger_msg.uTimestamp = (uint32_t)xTaskGetTickCount();
				msg.KeyTrigger_msg.IO_Status = 0;
				LoggingTask_Post(&msg);
			}
		}

		if (isGetBatteryStateFlag)
		{
			if (timeCountPer100ms % PERIOD_3_S == 0)
			{
				remaincap = UpdataBatteryCapacityAndDisplayLedState(remaincap);
			}
		}

		timeCountPer100ms++;
		//		uint8_t aa = Input_GetValue(IO_GPS_KEY);
		//		led_on++;
		//		if(led_on > 2)
		//		{
		//			led_on = 0;
		//		}
		//		GPIO_OutputCtrl(IO_LED_BAT_GREEN, led_on);//3 ALL
		//		GPIO_OutputCtrl(IO_LED_BAT_YELLOW, led_on);//4 2G
		//		GPIO_OutputCtrl(IO_LED_BAT_RED, led_on);//7  //2-4
		//		GPIO_OutputCtrl(IO_LED_ALL, led_on);//6
		//		GPIO_OutputCtrl(IO_LED_2G, led_on);//9
		//		GPIO_OutputCtrl(IO_LED_2_4G, led_on);//8 BAT GREEN
		//		GPIO_OutputCtrl(IO_LED_4_6G, led_on);//5 BAT YELLOW
		//		GPIO_OutputCtrl(IO_LED_GNSS, led_on);//10 ALARM
		//		GPIO_OutputCtrl(IO_LED_WARNING, led_on);//1 4-6
		//		GPIO_OutputCtrl(IO_LED_POWER, led_on);//2 GNSS
	}
}

static void TimerKeyCallback(void)
{
	static uint8_t count = 0;
	count++;
	if (count % 2 == 1)
	{
		GPIO_OutputCtrl(IO_LED_BAT_YELLOW, LED_ON); // yellow on
	}
	else
	{
		GPIO_OutputCtrl(IO_LED_BAT_YELLOW, LED_OFF); // yellow off
	}

	if (count >= 2)
	{
		count = 0;
	}
}

static void TimerRotateCallback(void)
{
	RotateStopRes = 0;
}

static void TimerHitTimeout_Callback(void)
{
	uHitStatus = 0;
	ClearUAVWarnSlideWinInfo();
	RotateStopRes = 0;
	LOG_DEBUG("Hit Timeout eEVENT_CLOSE_HIT\r\n");
	xTimerStop(TimerKeyHandle, 10);
	isGetBatteryStateFlag = true;
	HitProcessTask_Post(eEVENT_CLOSE_HIT);
	uHitKeyRelease = !uHitKeyRelease;
}

int32_t InitKeyHandle_Task()
{
	int32_t retVal = -1;

	xKeyHandleQueue = xQueueCreate(1, sizeof(uint32_t));

	TimerKeyHandle = xTimerCreate((const char *)"Timer",
								  (TickType_t)150,
								  (UBaseType_t)pdTRUE,
								  (void *)1,
								  (TimerCallbackFunction_t)TimerKeyCallback);
	if (TimerKeyHandle == NULL)
	{
		return retVal;
	}

	TimerRotateHandle = xTimerCreate((const char *)"Timer",
									 (TickType_t)150,
									 (UBaseType_t)pdFALSE,
									 (void *)1,
									 (TimerCallbackFunction_t)TimerRotateCallback);
	if (TimerRotateHandle == NULL)
	{
		return retVal;
	}

	TimerHitTimeout_Handle = xTimerCreate((const char *)"Timer",
										  (TickType_t)180000,
										  (UBaseType_t)pdFALSE,
										  (void *)2,
										  (TimerCallbackFunction_t)TimerHitTimeout_Callback);
	if (TimerHitTimeout_Handle == NULL)
	{
		return retVal;
	}

	if (xTaskCreate(KeyHandle_Task, "KeyHandle_Task", TASK_STACK_SIZE_KEYHANDLE, NULL, TASK_PRI_KEYHANDLE, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
