
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "netif/xadapter.h"

#include "data_path.h"

#include "../../hal/hal.h"
#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../srv/protocol/protocol_dbgdat.h"
#include "../alg/alg_init.h"
#include "../alg/det_alg/droneSniffer.h"
#include "../app_init.h"
#include "../logging/logging.h"
#include "../process_task/detect_process_task.h"
#include "../process_task/hit_process_task.h"
#include "../rf_config/rf_config.h"

uint8_t buffer_ADC_[RAW_ADC_DATA_LEN_PER_FRAME] __attribute__((section(".dmaSection")));
uint8_t buffer_ADC1_[RAW_ADC_DATA_LEN_PER_FRAME] __attribute__((section(".dmaSection")));
// uint64_t buffer_aeag_[RAW_ADC_DATA_LEN_PER_FRAME / sizeof(uint64_t)] __attribute__((section(".dmaSection")));
// uint64_t buffer_aeag1_[RAW_ADC_DATA_LEN_PER_FRAME / sizeof(uint64_t)] __attribute__((section(".dmaSection")));
#define NET_MAX_NUM 50
#define LINE_NUM 2343
#define COLUMN_NUM 128
#define REV_DET_PACK_LEN (128 * 2343 * 4)
uint32_t buffer_aeag_[1][LINE_NUM][COLUMN_NUM];
uint16_t buffer_aeag1_[NET_MAX_NUM][LINE_NUM][COLUMN_NUM];
// uint32_t detalgdata[1][LINE_NUM][COLUMN_NUM] = {0};
uint32_t detalgdata[1][LINE_NUM][COLUMN_NUM] = {0};
#define REV_DET_ALGDATA_LEN (32 * 1024)
#define MaxUAV 20

static uint64_t uDmaRevCount = 0;
static uint8_t uRevFlag = 6;
static uint8_t uDataProcessing = 0;

static QueueHandle_t sQueueHandle;
static s32 sFrameID = 0;
static s32 sDataPathOutEn = 0;
// for ADC
static DataPingPong_t sAdcBuf = {0};
static protocol_adc_data_t sAdcSendBuf = {0};

// for rdm
static DataPingPong_t sRdmBuf = {0};
static DataPathEvCb pDetectCb = NULL;

extern enum ethernet_link_status eth_link_status;
static sLogging_msg_t msg;

uint8_t uOutputSpecMat = 0;
void SetOutputSpecMat(uint8_t value)
{
	uOutputSpecMat = value;
}

uint8_t GetOutputSpecMat(void)
{
	return uOutputSpecMat;
}

void set_buffer_empty(s32 type, void *data)
{
	buffer_t *pBuff = NULL;

	if (ADC_DATA == type)
		pBuff = &sAdcBuf.buff1;

	pBuff = (pBuff->pData == (u8 *)data) ? (&sAdcBuf.buff1) : (&sAdcBuf.buff2);

	pBuff->flag = BUFF_EMPTY;
}
void printResult2(struct droneResult *droneInfo, int nDrone)
{
	uint32_t uDroneCnt = 0;
	//	if (nDrone > 0)
	//	{
	//		if (GetAlgorithmStopFlag() == 0)
	//		{
	//			HitProcessTask_Post(eEVENT_WARNING_LEVAL2);
	//			StartTimerWaringTimeout();
	//		}
	//	}
	for (int i = 0; i < nDrone; i++)
	{
		LOG_PRINTF("---------------------------------\n");
		if (droneInfo[i].flag == 0)
		{
			// continue;
			LOG_PRINTF("drone:\n");
			uDroneCnt++;
		}
		else
		{
			LOG_PRINTF("remote:\n");
		}

		LOG_PRINTF("ID=%d\n", droneInfo[i].ID);
		LOG_PRINTF("name=%s\n", droneInfo[i].name);
		LOG_PRINTF("amp=%.1f\n", droneInfo[i].amp);
		LOG_PRINTF("freq=%.1f\n", droneInfo[i].freq[0]);
		LOG_PRINTF("bandWidth=%.1f\n", droneInfo[i].bw);
		LOG_PRINTF("range=%.1f\n", droneInfo[i].range);

		msg.uLogNum = eLOG_DETECT_OUTSOURCE_INFO;
		msg.sDetectOutsourceInfo_msg.uTimestamp = (uint32_t)xTaskGetTickCount();
		msg.sDetectOutsourceInfo_msg.flag = droneInfo[i].flag;
		memcpy(msg.sDetectOutsourceInfo_msg.name, droneInfo[i].name, 20);
		msg.sDetectOutsourceInfo_msg.freq = droneInfo[i].freq[0];
		msg.sDetectOutsourceInfo_msg.bw = droneInfo[i].bw * 1000;
		LoggingTask_Post(&msg);
	}
//	if (uDroneCnt > 0)
	if (nDrone > 0)
	{
		if (GetAlgorithmStopFlag() == 0)
		{
			HitProcessTask_Post(eEVENT_WARNING_LEVAL2);
			StartTimerWaringTimeout();
		}
	}
}
/**
 * 1、监控pl状态是否正常
 * 2、处理pl过来的数据，上报或者调用后处理算法
 * 3、pl异常后，使其恢复。并上报状态
 */
void data_path_task(void *pvParameters)
{
	struct droneResult droneInfo[MaxUAV];
	int nDrone = 0;
	float cenFreq = 0.f; // MHz
	float fs = 40.f;	 // M
	float gain = 10.0f;
	int framenum = 0;
	buffer_t BufMsg = {0};
	BaseType_t ret = pdFALSE;
	int warn = 0;
	int drnum = 0;
	float ***specMat = createMat3(1, 2343, 128);
	//    float ***specMat=createMat(1,2343,128);
	uint8_t cntNet = 0;
	while (1)
	{
		ret = xQueueReceive(sQueueHandle, &BufMsg, pdMS_TO_TICKS(1500));
		if (pdTRUE == ret)
		{
			if ((ADC_DATA == BufMsg.type) && (ADC_DATA == sDataPathOutEn))
			{
			}
			else if ((RDMAP_DATA == BufMsg.type) && (RDMAP_DATA == sDataPathOutEn))
			{
				for (int i = 0; i < LINE_NUM; i++)
				{
					for (int j = 0; j < COLUMN_NUM; j++)
					{
						//						specMat[0][i][j]=(float)rand();
						specMat[0][i][j] = (float)detalgdata[0][i][j];
						//							xil_printf("%d  ", detalgdata[0][i][j]);
						buffer_aeag1_[cntNet][i][j] = (uint16_t)detalgdata[0][i][j];
						//						buffer_aeag1_[0][i][j] = cnt++;
					}
					//					xil_printf("\r\n");
				}

				framenum++;
				//对接收的一帧数据进行处理
				cenFreq = get_rx_freq(GetDetectFreqItem());
				axi_write_data(0x01, 0);
				Rf_Ad9361_cfg_f();
				memset((uint8_t*)&droneInfo, 0xFF, sizeof(droneInfo));
//				LOG_DEBUG("droneSniffer start\r\n");
				float bw = 34.0f;//M
				droneSniffer(droneInfo, &nDrone, (float***)specMat, cenFreq, fs, bw, gain);
//				LOG_DEBUG("droneSniffer end\r\n");
				if (GetOutputSpecMat() != 0)
				{
					if ((framenum >= 2) && (((nDrone >= 1) && (GetOutputSpecMat() == 2)) || (GetOutputSpecMat() == 1)))
					{
						LOG_DEBUG("%%%%%%%%%%%%%%%%%%%%%\r\n");
						cntNet++;
						if (cntNet >= NET_MAX_NUM)
						{
							protocol_udp_send_data2(buffer_aeag1_, sizeof(buffer_aeag1_));
							cntNet = 0;
							memset(buffer_aeag1_, 0, sizeof(buffer_aeag1_));
						}
					}
				}

				if (nDrone >= 20)
				{
					nDrone = 0;
				}
				printResult2(droneInfo, nDrone);

				if (nDrone >= 1)
				{
					warn++;
				}

				for(uint8_t i = 0; i < nDrone; i++)
				{
					drnum++;
					if (droneInfo[i].flag == 0)
					{
						LOG_DEBUG("fnum:%d  nDrone:%d  Freq:%.1f  warn:%d  drnum: %d  drone  %s\r\n", framenum, nDrone, cenFreq, warn, drnum, droneInfo[i].name);
					}
					else
					{
						LOG_DEBUG("fnum:%d  nDrone:%d  Freq:%.1f  warn:%d  drnum: %d  remote  %s\r\n", framenum, nDrone, cenFreq, warn, drnum, droneInfo[i].name);
					}

				}
//				if (((0 == droneInfo[0].flag) || (0 == droneInfo[1].flag)) && (nDrone >= 1) && ((strncmp(droneInfo[0].name, "DJI Mavic2", 10) == 0) || (strncmp(droneInfo[1].name, "DJI Mavic2", 10) == 0)))
//				{
//					drnum++;
//					LOG_DEBUG("fnum:%d  nDrone:%d  Freq:%.1f  warn:%d  drnum: %d   *** Mavic2 ***\r\n", framenum, nDrone, cenFreq, warn, drnum);
//				}
//				if (((0 == droneInfo[0].flag) || (0 == droneInfo[1].flag)) && (nDrone >= 1) && ((strncmp(droneInfo[0].name, "DJI Mavic3", 10) == 0) || (strncmp(droneInfo[1].name, "DJI Mavic3", 10) == 0)))
//				{
//					drnum++;
//					LOG_DEBUG("fnum:%d  nDrone:%d  Freq:%.1f  warn:%d  drnum: %d   *** Mavic3 ***\r\n", framenum, nDrone, cenFreq, warn, drnum);
//				}
//				if (((0 == droneInfo[0].flag) || (0 == droneInfo[1].flag)) && (nDrone >= 1) && ((strncmp(droneInfo[0].name, "DJI Phantom", 11) == 0) || (strncmp(droneInfo[1].name, "DJI Phantom", 11) == 0)))
//				{
//					drnum++;
//					LOG_DEBUG("fnum:%d  nDrone:%d  Freq:%.1f  warn:%d  drnum: %d   *** Phantom ***\r\n", framenum, nDrone, cenFreq, warn, drnum);
//				}
				if (nDrone == 0)
				{
					LOG_DEBUG("fnum:%d  nDrone:%d  Freq:%.1f  warn:%d  drnum: %d\r\n", framenum, nDrone, cenFreq, warn, drnum);
				}
				sRdmBuf.buff1.flag = BUFF_EMPTY;
				sRdmBuf.buff1.length = 0;
				uDataProcessing = 0;
				sRdmBuf.revLen = 0;
				Clear_RdmapDmaDataCount();
//				memset(buffer_aeag_, 0, sizeof(buffer_aeag_));
				axi_write_data(0x01, 1);
			}
			uDataProcessing = 0;
		}
		else
		{
			LOG_DEBUG("data_path timeout!\r\n");
		}
	}
	//	freeMat(specMat, 1, 2343);
}

void data_path_buff_init(s32 type)
{
	if (ADC_DATA == type)
	{
		sAdcBuf.revLen = 0;
		sAdcBuf.buff1.flag = BUFF_EMPTY;
		sAdcBuf.buff1.length = RAW_ADC_LEN;
		sAdcBuf.buff1.type = ADC_DATA;
		sAdcBuf.buff1.ts = 0;
		//		sAdcBuf.buff1.pData = (u8 *)ADC_BASE_ADDR;
		sAdcBuf.buff1.pData = (u8 *)&buffer_ADC_;

		sAdcBuf.buff2 = sAdcBuf.buff1;
		//		sAdcBuf.buff2.pData = (u8 *)(sAdcBuf.buff1.pData + RAW_ADC_LEN);
		sAdcBuf.buff2.pData = (u8 *)&buffer_ADC1_;
	}
	else if (RDMAP_DATA == type)
	{
		sRdmBuf.revLen = 0;
		sRdmBuf.buff1.flag = BUFF_EMPTY;
		sRdmBuf.buff1.length = REV_DET_PACK_LEN;
		sRdmBuf.buff1.type = RDMAP_DATA;
		sRdmBuf.buff1.ts = 0;
		//		sRdmBuf.buff1.pData = (u8 *)RDM_BASE_ADDR;
		sRdmBuf.buff1.pData = (u8 *)&buffer_aeag_;

		//		//		sRdmBuf.buff2 = sAdcBuf.buff1; buff2.type == 1 ??!
		//		memcpy(&sRdmBuf.buff2, &sRdmBuf.buff1, sizeof(sRdmBuf.buff1));
		////		sRdmBuf.buff2.pData = (u8 *)(sRdmBuf.buff1.pData + RDMAP_DATA_LEN);
		//		sRdmBuf.buff2.pData = (u8*)&buffer_aeag1_;
	}
}

buffer_t *data_path_getUsedBuf(s32 type)
{
	DataPingPong_t *pPing = NULL;
	buffer_t *pBuf = NULL;

	if (ADC_DATA == type)
	{
		pPing = &sAdcBuf;
	}
	else if (RDMAP_DATA == type)
	{
		pPing = &sRdmBuf;
	}
	else
	{
		;
	}

	if (pPing != NULL)
	{
		//	if ((BUFF_EMPTY == pPing->buff2.flag) &&
		//		(BUFF_EMPTY == pPing->buff1.flag))
		{
			pPing->usedId = 1;
		}
	}

	pBuf = (1 == pPing->usedId) ? (&pPing->buff1) : (&pPing->buff2);

	return pBuf;
}

buffer_t *data_path_getFullBuf(s32 type)
{
	DataPingPong_t *pPing = NULL;
	buffer_t *pBuf = NULL;

	if (ADC_DATA == type)
	{
		pPing = &sAdcBuf;
	}
	else if (RDMAP_DATA == type)
	{
		pPing = &sRdmBuf;
	}
	pBuf = (1 == pPing->usedId) ? (&pPing->buff2) : (&pPing->buff1);

	if (BUFF_FULL != pBuf->flag)
		pBuf = NULL;

	return pBuf;
}

uint64_t g_QueueSendFromISRFailCnt = 0;
void ADCDataCallback(s32 type, s32 len)
{
	portBASE_TYPE taskWoken = pdFALSE;
	BaseType_t ret = pdFALSE;
	buffer_t *pBuf = (1 == sAdcBuf.usedId) ? (&sAdcBuf.buff1) : (&sAdcBuf.buff2);

	sAdcBuf.revLen += (u32)len;
	if (RAW_ADC_LEN <= sAdcBuf.revLen)
	{
		sAdcBuf.usedId = (1 == sAdcBuf.usedId) ? 2 : 1;
		sAdcBuf.revLen = 0;
		pBuf->flag = BUFF_FULL;
	}
	if (BUFF_FULL == pBuf->flag)
	{
		sFrameID++;

		// data_trans_CacheInvalidate(pBuf, RAW_ADC_LEN);
		ret = xQueueSendFromISR(sQueueHandle, pBuf, &taskWoken);
		//		if (pdTRUE != ret)
		//			LOG_DEBUG("error!\n");
		if (pdTRUE == taskWoken)
		{
			portYIELD_FROM_ISR(taskWoken);
		}
		if (ret != pdTRUE)
		{
			// LOG_DEBUG("[%s:%d] %d\r\n", __FUNCTION__, __LINE__, ret);
			g_QueueSendFromISRFailCnt++;
		}

		pBuf = (1 == sAdcBuf.usedId) ? (&sAdcBuf.buff1) : (&sAdcBuf.buff2);
		if (BUFF_EMPTY != pBuf->flag)
		{
			pBuf->flag = BUFF_EMPTY; // reset flag
		}
	}

	data_trans_start(ADC_DATA, pBuf->pData + sAdcBuf.revLen, ADC_PACKET_LEN);
}

void RDMAPDataCallback(s32 type, s32 len)
{
	portBASE_TYPE taskWoken = pdFALSE;
	buffer_t *pBuf = &sRdmBuf.buff1;

	data_trans_CacheInvalidate(pBuf->pData + sRdmBuf.revLen, 199936);
	if (sDataPathOutEn == RDMAP_DATA)
	{
		uDmaRevCount++;
		if ((uDmaRevCount >= 6))
		{
			uDmaRevCount = 0;
			uRevFlag = 6;
		}
	}
	if (uDataProcessing == 1)
	{
		sRdmBuf.revLen = 0;
		len = 0;
	}
	if (uRevFlag)
	{
		sRdmBuf.revLen += (u32)len;
		if (REV_DET_PACK_LEN <= sRdmBuf.revLen)
		{
			sRdmBuf.revLen = 0;
			pBuf->flag = BUFF_FULL;
		}
		if (BUFF_FULL == pBuf->flag)
		{
			sFrameID++;
			//			if(sFrameID)
			if (uDataProcessing == 0)
			{
				uDataProcessing = 1;
				memcpy(&detalgdata[0][0][0], buffer_aeag_, REV_DET_PACK_LEN);
				xQueueSendFromISR(sQueueHandle, pBuf, &taskWoken);
				if (pdTRUE == taskWoken)
				{
					portYIELD_FROM_ISR(taskWoken);
				}
			}
		}
		uRevFlag--;
	}
	data_trans_start(RDMAP_DATA, pBuf->pData + sRdmBuf.revLen, 199936);
	//	data_trans_start(RDMAP_DATA, pBuf->pData, 199936);
}

s32 data_path_init(void)
{
	s32 status = 0;

	data_path_buff_init(ADC_DATA);
	data_path_buff_init(RDMAP_DATA);

	status = data_trans_init(ADC_DATA);
	if (!status)
		status = data_trans_set_callback(ADC_DATA, ADCDataCallback);

	if (!status)
		status = data_trans_init(RDMAP_DATA);
	if (!status)
		status = data_trans_set_callback(RDMAP_DATA, RDMAPDataCallback);

	sQueueHandle = xQueueCreate(10, sizeof(buffer_t));
	if (NULL == sQueueHandle)
	{
		status = -1;
	}

	xTaskCreate(data_path_task, "dataPathTask", TASK_STACK_SIZE_DATA_PATH, NULL, TASK_PRI_DATA_PATH, NULL);

	return status;
}

s32 data_path_start(s32 type)
{
	s32 status = 0;
	buffer_t *pBuf = NULL;

	if (ADC_DATA == type)
	{
		pBuf = data_path_getUsedBuf(ADC_DATA);
		if (pBuf)
			status = data_trans_start(ADC_DATA, pBuf->pData, ADC_PACKET_LEN);
	}
	else if (RDMAP_DATA == type)
	{
		pBuf = data_path_getUsedBuf(RDMAP_DATA);
		if (pBuf)
			//			status = data_trans_start(RDMAP_DATA, pBuf->pData, RDM_PACKET_LEN);// 199936
			status = data_trans_start(RDMAP_DATA, pBuf->pData, 199936);
	}

	return status;
}

s32 data_path_stop(s32 type)
{
	return 0;
}

void data_path_out_en(s32 type)
{
	sDataPathOutEn = type;
}

s32 data_path_setEvCallBack(DataPathEvCb pCb)
{
	pDetectCb = pCb;
	return 0;
}

void dma_transfer_adc_init()
{
	data_path_init();
	//	data_path_start(ADC_DATA); // adc
	//	data_path_out_en(ADC_DATA);

	data_path_start(RDMAP_DATA); //
	data_path_out_en(RDMAP_DATA);
}

void Clear_RdmapDmaDataCount()
{
	//	uDmaRevCount = 0;
	//	uRevFlag = 0;
	//	sRdmBuf.buff1.flag = BUFF_EMPTY;
}
