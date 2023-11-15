/*
 * display.c
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
/* FreeRTOS includes. */
#include "rf_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <timers.h>
#include "../../cfg/aeag_config.h"
#include "../../hal/flash_hal/flash_hal.h"
#include "../../hal/hal.h"
#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../hal/hal_ad9361/zynq_bram.h"
#include "../../hal/input/input.h"
#include "../../srv/log/log.h"
#include "../alg/detection/detection_interface.h"
#include "../alg/alg_init.h"
#include "../logging/logging.h"
#include "../DataPath/data_path.h"
#include "../switch_filter/switch_filter.h"

static TimerHandle_t TimerAD9361Init = NULL;
static QueueHandle_t xRf_ConfigQueue = NULL;
static sLogging_msg_t msg;
void Rf_Ad9361Sweep_f(void);
void Rf_ConfigTask_Post(uint32_t flag)
{
	if (xRf_ConfigQueue != NULL)
	{
		xQueueReset(xRf_ConfigQueue);
		xQueueSend(xRf_ConfigQueue, &flag, 10);
	}
}

static int32_t Rf_ConfigTask_Pend(uint32_t *flag, uint32_t timeout)
{

	int32_t result = -1;

	if (xRf_ConfigQueue != NULL)
	{
		if (xQueueReceive(xRf_ConfigQueue, flag, timeout))
		{
			result = 0;
		}
	}

	return result;
}

static uint8_t uDetectFreqItem = Freq_RX_2420m;
void SetDetectFreqItem(uint8_t value)
{
	if (value < Freq_MAX_CNT)
	{
		uDetectFreqItem = value;
	}
	else
	{
		uDetectFreqItem = 0;
	}
}

uint8_t GetDetectFreqItem(void)
{
	return uDetectFreqItem;
}

static uint8_t uSweepFlag = 1;
void SetSweepFlag(uint8_t value)
{
	uSweepFlag = value;
}

static void TimerAD9361InitCallback(void)
{
	msg.uLogNum = eLOG_AD9361_STAUTS;
	msg.Ad9361Status_msg.status = 1;
	LoggingTask_Post(&msg);
}

void Rf_Config_Task(void *p_arg)
{
	uint32_t flag;
	sPL_SignalHandlingCfg *sPL_Cfg;
	uint32_t PL_Version_date;
	uint32_t PL_Version_time;

	SetSweepFlag(0);
	/*After waiting for the PL side to run normally*/
	for (uint32_t i = 0; i < 100000; i++)
	{
		if (axi_read_data(0x08) == 1)
		{
			axi_write_data(0x00, 1);
			break;
		}
		vTaskDelay(1);
	}
	PL_Version_date = axi_read_data(0x09); /*set threshold*/
	PL_Version_time = axi_read_data(0x0A); /*set threshold*/
	axi_write_data(0x51, 2);			   /*set threshold*/

	InitAd9361_Rx();
	//	SimulaAdp_Init();
	axi_write_data(0x02, 69899369); /*set threshold*/
	axi_write_data(0x52, 1600);		/*set threshold*/
	axi_write_data(0x01, 1);
	SetSweepFlag(1);

	DacVotlCfg_init();
	DacRegCfg_init();

	sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();
	sPL_Cfg->uLocalOscFreq = set_rx_freq(uDetectFreqItem);
	axi_write_data(0x07, uDetectFreqItem);
	SimulaAdp_SendSerninCmd(eHMC1119_SERNIN6);
	SimulaAdp_SendAdrf5250md(eRF5250_CHANNEL3);
	if ((sPL_Cfg->uLocalOscFreq >= 2200) && (sPL_Cfg->uLocalOscFreq <= 2500))
	{
		//		SimulaAdp_SendAdrf5250md(eRF5250_CHANNEL3);
	}
	else if ((sPL_Cfg->uLocalOscFreq >= 5000) && (sPL_Cfg->uLocalOscFreq <= 5900))
	{
		//		SimulaAdp_SendAdrf5250md(eRF5250_CHANNEL5);
	}
	else
	{
		//		SimulaAdp_SendAdrf5250md(eRF5250_CHANNEL3);
	}
	LOG_DEBUG("PL_Version: %X %X\r\n", PL_Version_date & 0xFFFF, PL_Version_time);
	while (1)
	{
		if (Rf_ConfigTask_Pend(&flag, 1000) == 0)
		{
			enum RF_EVENT rf_flag = (enum RF_EVENT)flag;
			switch (rf_flag)
			{
			case eRF_AD9361_CFG_F:
			{
				Rf_Ad9361Sweep_f();
				break;
			}
			case eRF_PL_START_COLLECT:
			{
				axi_write_data(0x01, 1);
				break;
			}
			case eRF_PL_STOP_COLLECT:
			{
				axi_write_data(0x01, 0);
				break;
			}
			case eRF_AD9361_CFG_HIT_915:
			{
				SetAlgorithmStopFlag(1);
				data_path_out_en(0);
				InitAd9361();
				Ad9361_fre_hop_stop();
				Ad9361_fre_hop_set(Freq_TX_915m);
				Ad9361_fre_hop_start();
				break;
			}
			case eRF_AD9361_CFG_HIT_GPS:
			{
				SetAlgorithmStopFlag(1);
				data_path_out_en(0);
				InitAd9361();
				Ad9361_fre_hop_stop();
				Ad9361_fre_hop_set(Freq_TX_GPS);
				Ad9361_fre_hop_start();
				break;
			}
			case eRF_AD9361_CFG_HIT_GPS_915:
			{
				SetAlgorithmStopFlag(1);
				data_path_out_en(0);
				InitAd9361();
				Ad9361_fre_hop_stop();
				Ad9361_fre_hop_set(Freq_TX_GPS_915m);
				Ad9361_fre_hop_start();
				break;
			}
			case eRF_AD9361_CFG_CLOSE_HIT:
			{
				//				reset_ad936x();
				Ad9361_fre_hop_stop();
				InitAd9361_Rx();
				SetAlgorithmStopFlag(0);
				axi_write_data(0x01, 1);
				data_path_out_en(4);
				break;
			}
			default:
				break;
			}
		}
	}
}

void Rf_Ad9361Sweep_f(void)
{
	static uint8_t channel = eRF5250_CHANNEL3;
	sPL_SignalHandlingCfg *sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();

	//	    axi_write_data(0x01, 0);
	uDetectFreqItem++;
	if (uDetectFreqItem >= Freq_MAX_CNT)
	{
		uDetectFreqItem = 0;
	}
	sPL_Cfg->uLocalOscFreq = get_rx_freq(uDetectFreqItem);
	LOG_DEBUG("sPL_Cfg->uLocalOscFreq:  %d \r\n", sPL_Cfg->uLocalOscFreq);

	if ((sPL_Cfg->uLocalOscFreq >= 2200) && (sPL_Cfg->uLocalOscFreq <= 2500))
	{
		if (channel != eRF5250_CHANNEL3)
		{
			channel = eRF5250_CHANNEL3;
			SimulaAdp_SendAdrf5250md(eRF5250_CHANNEL3);
		}
	}
	else if ((sPL_Cfg->uLocalOscFreq >= 5000) && (sPL_Cfg->uLocalOscFreq <= 5900))
	{
		if (channel != eRF5250_CHANNEL5)
		{
			channel = eRF5250_CHANNEL5;
			SimulaAdp_SendAdrf5250md(eRF5250_CHANNEL5);
		}
	}
	else
	{
		SimulaAdp_SendAdrf5250md(eRF5250_CHANNEL3);
	}
	if (TimerAD9361Init)
	{
		xTimerStart(TimerAD9361Init, 10);
	}
	set_rx_freq(uDetectFreqItem);
	//		axi_write_data(0x01, 1);
	if (TimerAD9361Init)
	{
		xTimerStop(TimerAD9361Init, 10);
	}
}

void Rf_Ad9361_cfg_f(void)
{
	if ((GetAlgorithmStopFlag() == 0) && (uSweepFlag == 1))
	{
		Rf_Ad9361Sweep_f();
	}
}

int32_t InitRf_Config_Task()
{
	int32_t retVal = -1;

	xRf_ConfigQueue = xQueueCreate(1, sizeof(uint32_t));

	TimerAD9361Init = xTimerCreate((const char *)"TimerRf",
								   (TickType_t)5000,
								   (UBaseType_t)pdFALSE,
								   (void *)1,
								   (TimerCallbackFunction_t)TimerAD9361InitCallback);
	if (TimerAD9361Init == NULL)
	{
		return retVal;
	}

	if (xTaskCreate(Rf_Config_Task, "Rf_Config_Task", TASK_STACK_SIZE_RF_CONFIG, NULL, TASK_PRI_RF_CONFIG, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
