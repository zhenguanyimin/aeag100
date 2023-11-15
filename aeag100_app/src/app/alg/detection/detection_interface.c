#include "../../../srv/log/log.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "math.h"
#include "semphr.h"
#include "queue.h"
#include "xtime_l.h"
#include "xil_printf.h"
#include "detection_interface.h"
#include "data_process_module.h"
#include "param_estimate_module.h"
#include "../../DataPath/data_path.h"
#include "../../../hal/hal_ad9361/ad9361_config.h"

	char g_det_version[DETEC_VERSION_STR_LEN] = DETEC_ALG_VERSION;
	static SemaphoreHandle_t handleSem = NULL;

	uint32_t gFrameID = 0;
	uint32_t gTimeStampCur = 0; /* time from system startup at new frame beginning */
	uint32_t gTimeStampLast = 0;

	uint64_t g_stftOut_pl2ps[STFT_OUT_PER_DMA_LEN] = {0};
	unsigned int g_numDetPoint = 0, g_numDetSig = 0, g_numDetSigFinal = 0;

	signalParameter g_sigParam[MAX_NUM_SIGNAL] = {0};
	pjieguoInfo g_pwjieguoInfo = {0};

	float g_paramEstThresholdMag = THRESHOLD_MAG;
	int32_t g_paramEstThresholdTime = THRESHOLD_TIME;
	int32_t g_paramEstThresholdFreq = THRESHOLD_FREQ;
	int32_t g_alg_debug_printf_en = NONE;
	int32_t g_alg_identify_tc_match_en = 1;
	int32_t g_identify_UAV_cnt_th = IDENTYFY_UAV_CNT_TH;
	int32_t g_slide_win_len = DEFAULT_SLIDE_WIN_LEN;
	int32_t g_warn_cnt_in_win_th = DEFAULT_WARN_CNT_IN_WIN_TH;

	void setAlgDebugPrintfEn(int printfEn)
	{
		g_alg_debug_printf_en = printfEn; // 0:not, 1:yes
	}

	void setAlgIdentifyTcMatchEn(int tcMatchEn)
	{
		g_alg_identify_tc_match_en = tcMatchEn; // 0:not, 1:yes
	}

	void setThresholdMag(int thresholdMag)
	{
		g_paramEstThresholdMag = (float)(thresholdMag * 0.001); // unit=0.001
	}

	void setThresholdTime(int thresholdTime)
	{
		g_paramEstThresholdTime = thresholdTime; // unit=1
	}

	void setThresholdFreq(int thresholdFreq)
	{
		g_paramEstThresholdFreq = thresholdFreq; // unit=1
	}

	void setThresholdIdentifyUavCnt(int thresholdCnt)
	{
		g_identify_UAV_cnt_th = thresholdCnt; // unit=1
	}

	void setSlideWinLen(int slideWinLen)
	{
		g_slide_win_len = slideWinLen; // unit=1
	}

	void setThresholdWarnCntInWin(int thresholdWarnCnt)
	{
		g_warn_cnt_in_win_th = thresholdWarnCnt; // unit=1
	}

	uint32_t getFrameID(void)
	{
		return gFrameID;
	}

	void setFrameID(uint32_t uiFrameID)
	{
		gFrameID = uiFrameID;
	}

	char *getDetAlgVersion()
	{
		return g_det_version;
	}

	pjieguoInfo *GetPwOutInfo(void)
	{
		return &g_pwjieguoInfo;
	}

	void DetectAlgInit(void)
	{
		handleSem = xSemaphoreCreateBinary();
		if (!handleSem)
		{
			LOG_ERROR("xSemaphoreCreateBinary error\r\n");
			return;
		}
		else
		{
		}
	}

	int32_t RunDetectAlg(uint8_t *pBuf, int32_t len, pjieguoInfo *p_pwjieguoInfo)
	{
		int32_t i = 0, j = 0, iTmp = 0;
		int32_t signalId = 0;		 // signal id
		float signalFreq = 0;		 // signal frequency, in MHz
		float signalBw = 0;			 // signal bandwidth, in MHz
		float signalAmp = 0;		 // signal amplitude, in dB
		float signalAcq = 0;		 // signal acquisition time, in us
		float signalDur = 0;		 // signal duration, in us
		uint8_t localOscFreqNum = 0; // local oscillator frequency number
		uint8_t signalOneGroupFlag = 0;
		uint64_t priBigCycEndFlag = 0;
		uint64_t priBigCycEndFlag_last = 0;
		int32_t flagUAV = 0;

		//	memcpy(&g_stftOut_pl2ps[0], pBuf, len);
		memcpy(&g_stftOut_pl2ps[0], pBuf, sizeof(uint64_t) * STFT_OUT_PER_DMA_LEN);
		for (i = 0; i < STFT_OUT_PER_DMA_LEN; i++)
		{
			signalOneGroupFlag = ((g_stftOut_pl2ps[i] >> PARA_OUT_LOC_OSC_NUM_OFFSET) & PARA_OUT_LOC_OSC_NUM_MASK);
			priBigCycEndFlag = ((g_stftOut_pl2ps[i] >> PARA_OUT_BIG_CYC_END_OFFSET) & PARA_OUT_BIG_CYC_END_MASK);

			if ((signalOneGroupFlag == PARA_SIG_ONE_GROUP_FLAG) && (priBigCycEndFlag != PARA_BIG_CYC_END_FLAG) && (priBigCycEndFlag != 0))
			{ /* start of one signal group */
				if (i < (STFT_OUT_PER_DMA_LEN - 4))
				{ /* prevent array bound out */
					iTmp = i;
					localOscFreqNum = ((g_stftOut_pl2ps[iTmp + 2] >> PARA_OUT_LOC_OSC_NUM_OFFSET) & PARA_OUT_LOC_OSC_NUM_MASK);
					if (localOscFreqNum >= Freq_MAX_CNT)
						continue;
					signalId = ((g_stftOut_pl2ps[iTmp] >> PARA_OUT_SIG_ID_OFFSET) & PARA_OUT_SIG_ID_MASK);
					signalFreq = (float)(((g_stftOut_pl2ps[iTmp] >> PARA_OUT_SIG_FREQ_OFFSET) & PARA_OUT_SIG_FREQ_MASK) / PARA_OUT_SIG_FREQ_SCALE);
					signalBw = (float)(((g_stftOut_pl2ps[iTmp + 1] >> PARA_OUT_SIG_BW_OFFSET) & PARA_OUT_SIG_BW_MASK) / PARA_OUT_SIG_BW_SCALE);
					signalAmp = (float)(((g_stftOut_pl2ps[iTmp + 1] >> PARA_OUT_SIG_AMP_OFFSET) & PARA_OUT_SIG_AMP_MASK) / PARA_OUT_SIG_AMP_SCALE);
					signalAcq = (float)(((g_stftOut_pl2ps[iTmp + 2] >> PARA_OUT_SIG_ACQ_OFFSET) & PARA_OUT_SIG_ACQ_MASK) / PARA_OUT_SIG_ACQ_SCALE);
					signalDur = (float)(((g_stftOut_pl2ps[iTmp + 3] >> PARA_OUT_SIG_DUR_OFFSET) & PARA_OUT_SIG_DUR_MASK) / PARA_OUT_SIG_DUR_SCALE);
					g_sigParam[g_numDetSig].id = signalId;
					g_sigParam[g_numDetSig].freq = signalFreq + (get_rx_freq(localOscFreqNum) - 20);
					g_sigParam[g_numDetSig].BW = signalBw;
					g_sigParam[g_numDetSig].amp = log10f(sqrtf(signalAmp));
					g_sigParam[g_numDetSig].acqTime = signalAcq;
					g_sigParam[g_numDetSig].dur = signalDur;
					g_numDetSig++;
					if (g_numDetSig == MAX_NUM_SIGNAL)
					{
						g_numDetSig = 0;
						break;
					}
				}
			}

			if ((priBigCycEndFlag == PARA_BIG_CYC_END_FLAG) && (priBigCycEndFlag_last != priBigCycEndFlag) && (priBigCycEndFlag_last != 0))
			{ /* end of one pri big cycle */
				if (g_alg_debug_printf_en == PRINT_PARA_ESTIMATE_RESULT)
				{
					for (j = 0; j < g_numDetSig; j++)
					{
						LOG_DEBUG("%d	%d	%d	%d	%d	%d	%lld\r\n",
								  gFrameID, g_sigParam[j].id, (uint32_t)(g_sigParam[j].freq * 1000), (uint32_t)(g_sigParam[j].BW * 1000),
								  (uint32_t)(g_sigParam[j].amp * 1000), (uint32_t)(g_sigParam[j].dur), (uint64_t)(g_sigParam[j].acqTime));
					}
				}
				flagUAV += RunDetectAlgBlocking(p_pwjieguoInfo);
			}
			priBigCycEndFlag_last = priBigCycEndFlag;
		}

		return flagUAV;
	}

	int32_t RunDetectAlgBlocking(pjieguoInfo *pwjieguoInfo)
	{
		XTime tCur1 = 0, tCur2 = 0, tCur3 = 0, tCur4 = 0;
		XTime tEnd1 = 0, tEnd2 = 0, tEnd3 = 0, tEnd4 = 0;
		uint32_t tUsed1 = 0, tUsed2 = 0, tUsed3 = 0, tUsed4 = 0;
		int flagUAV;

		XTime_GetTime(&tCur1);
		gTimeStampCur = tCur1 * 1000000 / (COUNTS_PER_SECOND);
		/* Initialization */
		//    memset((void *)g_sigParam, 0, g_numDetSig * sizeof(signalParameter));
		//    memset((void *)pwjieguoInfo->signalList, 0, g_numDetSigFinal * sizeof(pjieguo));
		//    g_numDetSig = 0;
		//    g_numDetSigFinal = 0;
		XTime_GetTime(&tEnd1);
		tUsed1 = (tEnd1 - tCur1) * 1000000 / (COUNTS_PER_SECOND);

		XTime_GetTime(&tCur2);
		/* Signal parameters estimation processing */
		//	g_numDetSig = paramEstiProcessing(g_stftOut, g_detPoint, g_sigParam);
		//	g_numDetSig = paramEstiProcessing_V1_1(g_stftOut, g_detPoint, g_sigParam, &g_numDetPoint);
		XTime_GetTime(&tEnd2);
		tUsed2 = (tEnd2 - tCur2) * 1000000 / (COUNTS_PER_SECOND);

		XTime_GetTime(&tCur3);
		/* Data processing */
		//	numDetSigFinal = preProcessing(numDetSig, g_sigParam, g_pwjieguo);
		g_numDetSigFinal = preProcessing_V1_1(g_numDetSig, g_sigParam, pwjieguoInfo);
		XTime_GetTime(&tEnd3);
		tUsed3 = (tEnd3 - tCur3) * 1000000 / (COUNTS_PER_SECOND);

		XTime_GetTime(&tCur4);
		/* Identification process */
		//	flagUAV = identifyProcessing(numDetSigFinal, g_pwjieguo);
		flagUAV = identifyProcessing_V1_1(g_numDetSigFinal, pwjieguoInfo);
		XTime_GetTime(&tEnd4);
		tUsed4 = (tEnd4 - tCur4) * 1000000 / (COUNTS_PER_SECOND);

		if (g_alg_debug_printf_en == PRINT_DATA_PROCESS_RESULT)
		{
			LOG_DEBUG("	gFrameID:%d	tUsed1:%d us	tUsed2:%d us	tUsed3:%d us	tUsed4:%d us\r\n\n", gFrameID, tUsed1, tUsed2, tUsed3, tUsed4);
		}
		gFrameID++;

		memset((void *)g_sigParam, 0, g_numDetSig * sizeof(signalParameter));
		memset((void *)pwjieguoInfo->signalList, 0, g_numDetSigFinal * sizeof(pjieguo));
		g_numDetSig = 0;
		g_numDetSigFinal = 0;

		return flagUAV;
	}

/*================================================================================================*/
#ifdef __cplusplus
}
#endif
