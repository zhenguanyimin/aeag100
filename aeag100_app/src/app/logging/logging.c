/*
 * Logging.c
 *
 *  Created on: 2022-12-03
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
#include "logging.h"

#include "../../cfg/aeag_config.h"
#include "../../drv/axiuart/sys_intr.h"
#include "../../srv/cli/serial.h"
#include "../../srv/flash_nv/flash_nv.h"
#include "../../srv/log/log.h"
static QueueHandle_t xLoggingQueue;
static uint32_t uLoggingIndex = 0;
static sLogging_msg_t msg;

#define LOGGING_BUFF_SIZE (500 * 1024)

#define LOGGING_START_ADDR (0xC19000)
#define LOGGING_END_ADDR (LOGGING_START_ADDR + LOGGING_BUFF_SIZE)
static uint32_t flashlogbuf[LOGGING_BUFF_SIZE / 4];

void LoggingTask_Post(sLogging_msg_t *msg)
{
	if (xLoggingQueue != NULL)
	{
		xQueueSend(xLoggingQueue, msg, 10);
	}
}

static int32_t LoggingTask_Pend(sLogging_msg_t *msg, uint32_t timeout)
{
	int32_t result = -1;

	if (xLoggingQueue != NULL)
	{
		if (xQueueReceive(xLoggingQueue, msg, timeout))
		{
			result = 0;
		}
	}

	return result;
}

static int32_t LoggingStoreToFlash(uint8_t *buf, uint16_t len)
{
	static uint32_t uStroeLen = 0;
	int32_t ret;

	if (uLoggingIndex + len >= LOGGING_BUFF_SIZE)
	{
		ClearLogging();
	}
	ret = FlashHal_WriteByBytes(LOGGING_START_ADDR + uLoggingIndex, buf, len);
	if (ret != 0)
	{
		uLoggingIndex += 128;
		uStroeLen = 128;
		ret = FlashHal_WriteByBytes(LOGGING_START_ADDR + uLoggingIndex, buf, len);
		LOG_DEBUG("Logging Store To Flash error\r\n");
	}

	uLoggingIndex += len;
	uStroeLen += len;
	if (uStroeLen > 128)
	{
		ret = FlashNV_Set(FLASH_NV_ID_LOGGING_INDEX, &uLoggingIndex, 4);
		uStroeLen = 0;
	}
	//	FlashHal_ReadByBytes(LOGGING_START_ADDR, (uint8_t*)flashlogbuf, 1000);

	return ret;
}

int32_t ClearLogging(void)
{
	int32_t ret;
	uLoggingIndex = 0;
	ret = FlashNV_Set(FLASH_NV_ID_LOGGING_INDEX, &uLoggingIndex, 4);
	for (uint32_t addr = LOGGING_START_ADDR; addr < LOGGING_END_ADDR; addr = addr + 4096)
	{
		FlashHal_Erase(addr, 4096);
	}

	return ret;
}

// static int32_t LoggingStrAnalyze(uint8_t *buf, uint16_t len)
//{
//	int32_t ret;
//	if (buf != NULL)
//	{
//		for (uint32_t i = 0; i < len; i++)
//		{
//
//		}
//	}
//	return ret;
// }

void LoggingProcess_Task(void *p_arg)
{
	uint8_t buf[100] = {0};
	uint32_t len_rd = 0;
	size_t len_save = 0;
	int32_t ret;

	len_rd = FlashNV_Get(FLASH_NV_ID_LOGGING_INDEX, &uLoggingIndex, 4, &len_save);
	if ((len_rd == sizeof(uLoggingIndex)) && (len_rd == len_save) && (uLoggingIndex != 0))
	{
	}
	else
	{
		ClearLogging();
	}
	buf[0] = '$';
	buf[1] = '$';
	msg.uLogNum = eLOG_VERSION_STR;
	msg.VersionStr_msg.len = strlen(EMBED_SOFTWARE_PS_VERSION_STR);
	memcpy(msg.VersionStr_msg.Str, EMBED_SOFTWARE_PS_VERSION_STR, msg.VersionStr_msg.len);
	LoggingTask_Post(&msg);
	while (1)
	{
		if (LoggingTask_Pend(&msg, 2000) == 0)
		{
			buf[2] = msg.uLogNum;
			switch (msg.uLogNum)
			{
			case eLOG_VERSION_STR:
			{
				memcpy(buf + 3, (uint8_t *)&msg.VersionStr_msg, sizeof(sVersionStr_msg_t));
				LoggingStoreToFlash(buf, sizeof(sVersionStr_msg_t) + 3);
				break;
			}
			case eLOG_AD9361_STAUTS:
			{
				memcpy(buf + 3, (uint8_t *)&msg.Ad9361Status_msg, sizeof(sAd9361Status_msg_t));
				LoggingStoreToFlash(buf, sizeof(sAd9361Status_msg_t) + 3);
				break;
			}
			case eLOG_START_HIT_FRE:
			{
				memcpy(buf + 3, (uint8_t *)&msg.StartHit_msg, sizeof(sStartHit_msg_t));
				LoggingStoreToFlash(buf, sizeof(sStartHit_msg_t) + 3);
				break;
			}
			case eLOG_END_HIT:
			{
				memcpy(buf + 3, (uint8_t *)&msg.EndHit_msg, sizeof(sEndHit_msg_t));
				LoggingStoreToFlash(buf, sizeof(sEndHit_msg_t) + 3);
				break;
			}
			case eLOG_HIT_CURRENT:
			{
				memcpy(buf + 3, (uint8_t *)&msg.HitCurrent_msg, sizeof(sHitCurrent_msg_t));
				LoggingStoreToFlash(buf, sizeof(sHitCurrent_msg_t) + 3);
				break;
			}
			case eLOG_ELECTRICITY:
			{
				memcpy(buf + 3, (uint8_t *)&msg.Electricity_msg, sizeof(sElectricity_msg_t));
				LoggingStoreToFlash(buf, sizeof(sElectricity_msg_t) + 3);
				break;
			}
			case eLOG_DETECT_INFO_FRE_BW_A:
			{
				memcpy(buf + 3, (uint8_t *)&msg.DetectInfo_msg, sizeof(sDetectInfo_msg_t));
				LoggingStoreToFlash(buf, sizeof(sDetectInfo_msg_t) + 3);
				break;
			}
			case eLOG_DETECT_POINTS_WARNIG:
			{
				memcpy(buf + 3, (uint8_t *)&msg.sDetectPointsWarnig_msg, sizeof(sDetectPointsWarnig_msg_t));
				LoggingStoreToFlash(buf, sizeof(sDetectPointsWarnig_msg_t) + 3);
				break;
			}
			case eLOG_KEY_TRIGGER:
			{
				memcpy(buf + 3, (uint8_t *)&msg.KeyTrigger_msg, sizeof(sKeyTrigger_msg_t));
				LoggingStoreToFlash(buf, sizeof(sKeyTrigger_msg_t) + 3);
				break;
			}
			case eLOG_DETECT_OUTSOURCE_INFO:
			{
				memcpy(buf + 3, (uint8_t *)&msg.sDetectOutsourceInfo_msg, sizeof(sDetectOutsourceInfo_msg_t));
				LoggingStoreToFlash(buf, sizeof(sDetectOutsourceInfo_msg_t) + 3);
				break;
			}
			case eLOG_DAC_PARAM:
			{
				memcpy(buf + 3, (uint8_t *)&msg.sDacParam_msg, sizeof(sDacParam_msg_t));
				LoggingStoreToFlash(buf, sizeof(sDacParam_msg_t) + 3);
				break;
			}
			case eLOG_LOGGING_OUTPUT:
			{
				for (uint32_t offset = 0; offset < uLoggingIndex; offset = offset + 4096)
				{
					ret = FlashHal_ReadByBytes(LOGGING_START_ADDR + offset, (uint8_t *)flashlogbuf + offset, 4096);
					if (ret != 0)
					{
						FlashHal_ReadByBytes(LOGGING_START_ADDR + offset, (uint8_t *)flashlogbuf + offset, 4096);
					}
				}
				if (ret == 0)
				{
					vTaskDelay(550);
					memset(buf, '%', 6);
					//					UartLite_SendData(0, (uint8_t*)buf, 6);
					//					vTaskDelay(5);
					//					UartLite_SendData(0, (uint8_t*)flashlogbuf, uLoggingIndex);
					//
					//					for (uint32_t i = 0; i < 2000; i++)
					//					{
					//						if (GetUartLiteInstSendStatus(0) == 0)
					//						{
					//							memset(buf, '%', 6);
					//							UartLite_SendData(0, (uint8_t*)buf, 6);
					//							break;
					//						}
					//						vTaskDelay(100);
					//					}

					LogUartPrint2((uint8_t *)buf, 6);
					vTaskDelay(5);
					LogUartPrint2((uint8_t *)flashlogbuf, uLoggingIndex);

					for (uint32_t i = 0; i < 2000; i++)
					{
						if (GetLogUartLiteInstSendStatus() == 0)
						{
							memset(buf, '%', 6);
							LogUartPrint2((uint8_t *)buf, 6);
							break;
						}
						vTaskDelay(100);
					}
					buf[0] = '$';
					buf[1] = '$';
				}
				break;
			}
			default:
				break;
			}
		}
	}
}

void LoggingTest(void)
{
	for (uint16_t i = 0; i < 100; i++)
	{
		msg.uLogNum = eLOG_VERSION_STR;
		msg.VersionStr_msg.len = strlen(EMBED_SOFTWARE_PS_VERSION_STR);
		memcpy(msg.VersionStr_msg.Str, EMBED_SOFTWARE_PS_VERSION_STR, msg.VersionStr_msg.len);
		LoggingTask_Post(&msg);
		vTaskDelay(30);
	}
	LOG_DEBUG("uLoggingIndex:%d\r\n", uLoggingIndex);
}

int32_t InitLoggingProcess_Task()
{
	int32_t retVal = -1;

	xLoggingQueue = xQueueCreate(10, sizeof(sLogging_msg_t));

	if (xTaskCreate(LoggingProcess_Task, "LoggingProcess_Task", TASK_STACK_SIZE_LOGGINGPROCESS, NULL, TASK_PRI_LOGGING, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
