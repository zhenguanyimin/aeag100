/*
 * detect_process_task.c
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
/* FreeRTOS includes. */
#include "detect_process_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <timers.h>
#include <xil_printf.h>

#include "../../cfg/aeag_config.h"
#include "../../hal/hal.h"
#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../inc/common_define.h"
#include "../../srv/log/log.h"
#include "../alg/alg_init.h"
#include "../alg/detection/detection_interface.h"
#include "../rf_config/rf_config.h"
#include "hit_process_task.h"
#include "../DataPath/data_path.h"
#include "../logging/logging.h"

static sLogging_msg_t msg;
#define SWEEP_TIME 200 // ms
static TimerHandle_t TimerCfg_f_Handle = NULL;
static TimerHandle_t TimerWaringTimeout_Handle = NULL;
uint8_t Cfg_f_flag = 0;
uint32_t g_algRunCnt = 0;
static sUAVWarnSlideWin g_UAVWarnSlideWinInfo = {0};
void ClearUAVWarnSlideWinInfo(void)
{
	memset((uint8_t *)g_UAVWarnSlideWinInfo.identiUavCnt, 0, DEFAULT_SLIDE_WIN_LEN_MAX * 4);
	g_UAVWarnSlideWinInfo.warnCntInWin = 0;
}

sSignalDescriptionList mSignalDescriptionList =
	{
		16,
		{
			{0, -10000, 10, 2440000000, 40000000, 40, 100},
			{1, -10000, 10, 2440000000, 40000000, 40, 100},
			{2, -10000, 10, 2480000000, 40000000, 30, 100},
			{3, -10000, 10, 2400000000, 40000000, 40, 100},
			{4, -10000, 10, 2480000000, 40000000, 30, 100},
			{5, -10000, 10, 2400000000, 40000000, 40, 100},
			{6, -10000, 10, 2480000000, 40000000, 30, 100},
			{7, -10000, 10, 2400000000, 40000000, 40, 100},
			{8, -10000, 10, 2480000000, 40000000, 30, 100},
			{9, -10000, 10, 2400000000, 40000000, 40, 100},
			{10, -10000, 10, 2460000000, 40000000, 50, 100},
			{11, -10000, 10, 2460000000, 40000000, 50, 100},
			{12, -10000, 10, 2400000000, 40000000, 40, 100},
			{13, -10000, 10, 2460000000, 40000000, 50, 100},
			{14, -10000, 10, 2460000000, 40000000, 50, 100},
			{15, -10000, 10, 2400000000, 40000000, 40, 100},
		},
};

static QueueHandle_t xDetectProcessQueue;

void DetectProcessTask_Post(uint32_t bufaddr)
{
	if (xDetectProcessQueue != NULL)
	{
		xQueueSend(xDetectProcessQueue, &bufaddr, 10);

		//		BaseType_t ret = 0 ;
		//	    BaseType_t xHigherPriorityTaskWoken ;
		//	    ret = xQueueIsQueueFullFromISR(xDetectProcessQueue);
		//	    if( pdTRUE != ret)
		//	    	xQueueSendFromISR(xDetectProcessQueue, canMsg, &xHigherPriorityTaskWoken ) ;
	}
}

static int32_t DetectProcessTask_Pend(uint32_t *bufaddr, uint32_t timeout)
{
	int32_t result = -1;

	if (xDetectProcessQueue != NULL)
	{
		if (xQueueReceive(xDetectProcessQueue, (void *)bufaddr, timeout))
		{
			result = 0;
		}
	}

	return result;
}

static int32_t UAVWarnSildeWinProcess(uint32_t algRunCnt, int32_t identifyUavCnt, sUAVWarnSlideWin *pUAVWarnSlideWin)
{
	int32_t i = 0;

	if (g_slide_win_len >= DEFAULT_SLIDE_WIN_LEN_MAX)
	{
		g_slide_win_len = DEFAULT_SLIDE_WIN_LEN_MAX - 1;
	}
	pUAVWarnSlideWin->slideWinLen = g_slide_win_len;

	if (algRunCnt < pUAVWarnSlideWin->slideWinLen)
	{
		pUAVWarnSlideWin->identiUavCnt[algRunCnt] = identifyUavCnt;
		if (pUAVWarnSlideWin->identiUavCnt[algRunCnt] >= g_identify_UAV_cnt_th)
		{
			pUAVWarnSlideWin->warnCntInWin++;
		}
	}
	else
	{
		pUAVWarnSlideWin->warnCntInWin = 0;
		pUAVWarnSlideWin->identiUavFlag = 0;
		pUAVWarnSlideWin->identiUavCnt[pUAVWarnSlideWin->slideWinLen] = identifyUavCnt;
		for (i = 0; i < pUAVWarnSlideWin->slideWinLen; i++)
		{
			pUAVWarnSlideWin->identiUavCnt[i] = pUAVWarnSlideWin->identiUavCnt[i + 1];
			if (pUAVWarnSlideWin->identiUavCnt[i] >= g_identify_UAV_cnt_th)
			{
				pUAVWarnSlideWin->warnCntInWin++;
			}
		}
	}

	if (pUAVWarnSlideWin->warnCntInWin >= g_warn_cnt_in_win_th)
	{
		pUAVWarnSlideWin->identiUavFlag = 1;
	}

	if (g_alg_debug_printf_en != PRINT_PARA_ESTIMATE_RESULT)
	{
		LOG_PRINTF("**** UAVWarnSildeWinProcess: algRunCnt:%d,  identiUavCntTh:%d,  warnCntInWinTH:%d,  slideWin:%d,  [",
				   g_algRunCnt, g_identify_UAV_cnt_th, g_warn_cnt_in_win_th, pUAVWarnSlideWin->slideWinLen);
		for (i = 0; i < pUAVWarnSlideWin->slideWinLen; i++)
		{
			LOG_PRINTF("%d  ", pUAVWarnSlideWin->identiUavCnt[i]);
		}
		LOG_PRINTF("]  warnCntInWin:%d,  identiUavFlag:%d\r\n", pUAVWarnSlideWin->warnCntInWin, pUAVWarnSlideWin->identiUavFlag);
	}
	else if (g_alg_debug_printf_en == PRINT_PARA_ESTIMATE_RESULT)
	{
		LOG_PRINTF("\n%d \r\n", g_algRunCnt);
	}

	return pUAVWarnSlideWin->identiUavFlag;
}

void DetectProcess_Task(void *p_arg)
{
	uint32_t sourceAddr = 0;
	//	sSignalDescriptionList *pSignalDescriptionList;
	//	output_UAV_List OutputList;
	uint8_t uLeval = 0;
	int32_t identifyUavCnt = 0;
	pjieguoInfo *p_PwOutInfo = GetPwOutInfo();
	DetectAlgInit();
	xTimerStart(TimerCfg_f_Handle, 10);
	float fDistance = 0.0;

	while (1)
	{
		if (DetectProcessTask_Pend(&sourceAddr, SWEEP_TIME) == 0)
		{
			if (sourceAddr != 0)
			{
				if (Cfg_f_flag)
				{
					Cfg_f_flag = 0;
					//					Rf_ConfigTask_Post(eRF_PL_STOP_COLLECT);
					//					Rf_ConfigTask_Post(eRF_AD9361_CFG_F);
					axi_write_data(0x01, 0);
					vTaskDelay(2);
					axi_write_data(0x07, GetDetectFreqItem());
					Rf_Ad9361_cfg_f();
					vTaskDelay(2);
					axi_write_data(0x01, 1);
					xTimerReset(TimerCfg_f_Handle, 10);
					xTimerStart(TimerCfg_f_Handle, 10);
				}
				//				LOG_DEBUG("algorithm_Run1......\r\n");
				identifyUavCnt = algorithm_Run1((uint8_t *)sourceAddr, RDMAP_DATA_LEN, p_PwOutInfo);

				//				pSignalDescriptionList = (sSignalDescriptionList *)sourceAddr;
				//				if (algorithm_Run(pSignalDescriptionList, &OutputList))
				//				if (identifyUavCnt > 0)
				{
					uLeval = 0;
					if (UAVWarnSildeWinProcess(g_algRunCnt, identifyUavCnt, &g_UAVWarnSlideWinInfo) && (GetAlgorithmStopFlag() == 0))
					{
						xTimerStart(TimerWaringTimeout_Handle, 10);
						fDistance = p_PwOutInfo->outputWarnDistUAV;
						if ((fDistance >= 0) && (fDistance <= WARNING_LEVAL3_DIST))
						{
							uLeval = 3;
							if (g_alg_debug_printf_en != PRINT_PARA_ESTIMATE_RESULT)
								LOG_DEBUG("eEVENT_WARNING_LEVAL3\r\n");
							HitProcessTask_Post(eEVENT_WARNING_LEVAL3);
						}
						else if ((fDistance > WARNING_LEVAL3_DIST) && (fDistance <= WARNING_LEVAL2_DIST))
						{
							uLeval = 2;
							if (g_alg_debug_printf_en != PRINT_PARA_ESTIMATE_RESULT)
								LOG_DEBUG("eEVENT_WARNING_LEVAL2\r\n");
							HitProcessTask_Post(eEVENT_WARNING_LEVAL2);
						}
						else if (fDistance > WARNING_LEVAL2_DIST)
						{
							uLeval = 1;
							if (g_alg_debug_printf_en != PRINT_PARA_ESTIMATE_RESULT)
								LOG_DEBUG("eEVENT_WARNING_LEVAL1\r\n");
							HitProcessTask_Post(eEVENT_WARNING_LEVAL1);
						}
					}
					msg.uLogNum = eLOG_DETECT_POINTS_WARNIG;
					msg.sDetectPointsWarnig_msg.uTimestamp = (uint32_t)xTaskGetTickCount();
					msg.sDetectPointsWarnig_msg.uWarningLeval = uLeval;
					msg.sDetectPointsWarnig_msg.uDetectPoints[0] = g_UAVWarnSlideWinInfo.identiUavCnt[0];
					msg.sDetectPointsWarnig_msg.uDetectPoints[1] = g_UAVWarnSlideWinInfo.identiUavCnt[1];
					msg.sDetectPointsWarnig_msg.uDetectPoints[2] = g_UAVWarnSlideWinInfo.identiUavCnt[2];
					msg.sDetectPointsWarnig_msg.uDetectPoints[3] = g_UAVWarnSlideWinInfo.identiUavCnt[3];
					msg.sDetectPointsWarnig_msg.uDetectPoints[4] = g_UAVWarnSlideWinInfo.identiUavCnt[4];
					LoggingTask_Post(&msg);
					g_algRunCnt++;
				}
				//				Rf_ConfigTask_Post(eRF_PL_START_COLLECT);
				//				g_algRunCnt++;
			}
		}
		else
		{
			//			Rf_ConfigTask_Post(eRF_PL_STOP_COLLECT);
			//			Rf_ConfigTask_Post(eRF_AD9361_CFG_F);
			xTimerReset(TimerCfg_f_Handle, 10);
			axi_write_data(0x01, 0);
			vTaskDelay(2);
			axi_write_data(0x07, GetDetectFreqItem());
			Rf_Ad9361_cfg_f();
			vTaskDelay(2);
			axi_write_data(0x01, 1);
			//			Rf_ConfigTask_Post(eRF_PL_START_COLLECT);
		}
	}
}

static void TimerCfg_f_Callback(void)
{
	Cfg_f_flag = 1;
}

static void TimerWaringTimeout_Callback(void)
{
	HitProcessTask_Post(eEVENT_NO_WARNING);
}

int32_t StartTimerWaringTimeout(void)
{
	int32_t retVal = -1;

	if (TimerCfg_f_Handle != NULL)
	{
		xTimerStart(TimerWaringTimeout_Handle, 10);
	}

	return retVal;
}

int32_t InitDetectProcess_Task()
{
	int32_t retVal = -1;
	int32_t *pvTimerID = (int32_t *)1;
	TimerCfg_f_Handle = xTimerCreate((const char *)"Timer",
									 (TickType_t)SWEEP_TIME,
									 (UBaseType_t)pdFALSE,
									 (void *)pvTimerID,
									 (TimerCallbackFunction_t)TimerCfg_f_Callback);

	if (TimerCfg_f_Handle == NULL)
	{
		return retVal;
	}

	TimerWaringTimeout_Handle = xTimerCreate((const char *)"Timer",
											 (TickType_t)12000,
											 (UBaseType_t)pdFALSE,
											 (void *)pvTimerID,
											 (TimerCallbackFunction_t)TimerWaringTimeout_Callback);

	if (TimerWaringTimeout_Handle == NULL)
	{
		return retVal;
	}

	xDetectProcessQueue = xQueueCreate(5, sizeof(uint32_t));

	//	if (xTaskCreate(DetectProcess_Task, "DetectProcess_Task", TASK_STACK_SIZE_DETECTPROCESS, NULL, TASK_PRI_DETECTPROCESS, NULL) == 1)
	//	{
	//		retVal = 0;
	//	}

	return retVal;
}
