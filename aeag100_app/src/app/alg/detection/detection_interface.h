#ifndef DETECTION_INIT_H_
#define DETECTION_INIT_H_

/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "system_param.h"
#include <stdio.h>

/***** Detection algorithm version  *****
det_01.110110
	 | --------- main detection algorithm version number
	   | ------- version number of wave type
		 | ------- version number of band width, adc sampling frequency, sampling time, fft window function ...
		  | ------- version number of paramEstiProcessing
		   | ------- version number of preProcessing of data process
			| ------- version number of identifyProcessing of data process
			 | ------- version number of detection interface
*/
#define DETEC_ALG_VERSION "DET_0.0023D2"
#define DETEC_VERSION_STR_LEN (14)
/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
#define STFT_OUT_T_IDX_OFFSET 41				 // 41bit start
#define STFT_OUT_T_IDX_MASK 0x7fff				 // 15bit valid
#define STFT_OUT_F_IDX_OFFSET 32				 // 32bit start
#define STFT_OUT_F_IDX_MASK 0x1ff				 // 9bit valid
#define STFT_OUT_AMP_OFFSET 0					 // 0bit start
#define STFT_OUT_AMP_MASK 0xffffffff			 // 32bit valid
#define PRI_BIG_CYC_END_FLAG_T_IDX 0x1fff		 // 15bit[55:41]
#define PRI_BIG_CYC_END_FLAG_F_IDX 0x1ff		 // 9bit[40:32]
#define PRI_BIG_CYC_END_FLAG_AMP 0x3a5a3a5a		 // 32bit[31:0]
#define PRI_BIG_CYC_END_FLAG 0x423fffff3a5a3a5a	 // 64bit[63:0]
#define STFT_OUT_PER_DMA_LEN 4096				 // 2048
#define STFT_OUT_SCALE_EXP 16					 // 2^6
#define STFT_OUT_SCALE (1 << STFT_OUT_SCALE_EXP) // 65536=2^16

#define PARA_OUT_LOC_OSC_NUM_OFFSET 56			   // 0bit start
#define PARA_OUT_LOC_OSC_NUM_MASK 0xff			   // 8bit[63:56] valid
#define PARA_OUT_SIG_ID_OFFSET 0				   // 0bit start
#define PARA_OUT_SIG_ID_MASK 0xffffffff			   // 32bit[31:0] valid
#define PARA_OUT_SIG_FREQ_OFFSET 32				   // 32bit start
#define PARA_OUT_SIG_FREQ_MASK 0xffffff			   // 24bit[55:32] valid
#define PARA_OUT_SIG_FREQ_SCALE 1024.0f			   // divide 1024.0 to real value
#define PARA_OUT_SIG_BW_OFFSET 0				   // 0bit start
#define PARA_OUT_SIG_BW_MASK 0xffffff			   // 24bit[23:0] valid
#define PARA_OUT_SIG_BW_SCALE 1024.0f			   // divide 1024.0 to real value
#define PARA_OUT_SIG_AMP_OFFSET 24				   // 24bit start
#define PARA_OUT_SIG_AMP_MASK 0xffffffffff		   // 40bit[63:24] valid
#define PARA_OUT_SIG_AMP_SCALE 256.0f			   // divide 256.0 to real value
#define PARA_OUT_SIG_ACQ_OFFSET 0				   // 0bit start
#define PARA_OUT_SIG_ACQ_MASK 0x3fffffffffff	   // 46bit[45:0] valid
#define PARA_OUT_SIG_ACQ_SCALE 512.0f			   // divide 512.0 to real value
#define PARA_OUT_SIG_DUR_OFFSET 0				   // 0bit start
#define PARA_OUT_SIG_DUR_MASK 0x3fffffffffff	   // 46bit[45:0] valid
#define PARA_OUT_SIG_DUR_SCALE 512.0f			   // divide 512.0 to real value
#define PARA_OUT_BIG_CYC_END_OFFSET 0			   // 0bit start
#define PARA_OUT_BIG_CYC_END_MASK 0xffffffffffffff // 56bit[55:0] valid

#define PARA_SIG_ONE_GROUP_FLAG 0xff		   // 8bit[63:56]
#define PARA_BIG_CYC_END_FLAG 0x3fffff3b5b3b5b // 56bit[55:0]

#define IDENTYFY_UAV_CNT_TH 15u
#define DEFAULT_SLIDE_WIN_LEN_MAX 20u /* default slide window length max */
#define DEFAULT_SLIDE_WIN_LEN 5u	  /* default slide window length */
#define DEFAULT_WARN_CNT_IN_WIN_TH 2u /* default threshold of warning count in slide window */

/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/
typedef struct sUAVWarnSlideWin_t
{
	int32_t slideWinLen;							 /* slide window length */
	int32_t warnCntInWin;							 /* warn count in slide window */
	int32_t identiUavFlag;							 /* identified UAV flag, 0:not, 1:yes */
	int32_t identiUavCnt[DEFAULT_SLIDE_WIN_LEN_MAX]; /* identified UAV count per detection algorithm cycle */
} sUAVWarnSlideWin;

typedef enum
{
	NONE = 0,						/* do not print algorithm result */
	PRINT_DATA_PROCESS_RESULT = 1,	/* only print data process result */
	PRINT_PARA_ESTIMATE_RESULT = 2, /* only print parameter estimate result */

} eAlgPrintType;

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern pjieguoInfo g_pwjieguoInfo;
extern float g_paramEstThresholdMag;
extern int32_t g_paramEstThresholdTime;
extern int32_t g_paramEstThresholdFreq;
extern int32_t g_alg_debug_printf_en;
extern int32_t g_alg_identify_tc_match_en;
extern int32_t g_identify_UAV_cnt_th;
extern int32_t g_slide_win_len;
extern int32_t g_warn_cnt_in_win_th;

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
extern void setAlgDebugPrintfEn(int printfEn);
extern void setAlgIdentifyTcMatchEn(int tcMatchEn);
extern void setThresholdMag(int thresholdMag);
extern void setThresholdTime(int thresholdTime);
extern void setThresholdFreq(int thresholdFreq);
extern void setThresholdIdentifyUavCnt(int thresholdCnt);
extern void setSlideWinLen(int slideWinLen);
extern void setThresholdWarnCntInWin(int thresholdWarnCnt);
extern uint32_t getFrameID(void);
extern void setFrameID(uint32_t uiFrameID);
extern char *getDetAlgVersion();
extern pjieguoInfo *GetPwOutInfo(void);

extern void DetectAlgInit(void);
extern int32_t RunDetectAlg(uint8_t *pBuf, int32_t len, pjieguoInfo *p_pwjieguoInfo);
int32_t RunDetectAlgBlocking(pjieguoInfo *pwjieguoInfo);

#endif /*DETECTION_INIT_H_*/
