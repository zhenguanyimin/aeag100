

/* FreeRTOS includes. */
#include "ut_alg_dbg.h"

#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "../../app/alg/detection/detection_interface.h"

s32 alg_debug_printf_en_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 printfEn = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		printfEn = strtoul(param1, NULL, 0);

		if (printfEn >= 0)
		{
			setAlgDebugPrintfEn(printfEn);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 alg_identify_tc_match_en_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 tcMatchEn = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		tcMatchEn = strtoul(param1, NULL, 0);

		if (tcMatchEn >= 0)
		{
			setAlgIdentifyTcMatchEn(tcMatchEn);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 alg_param_threshold_mag_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threshold_mag = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threshold_mag = strtoul(param1, NULL, 0);

		if (threshold_mag >= 0)
		{
			setThresholdMag(threshold_mag);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 alg_param_threshold_time_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threshold_time = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threshold_time = strtoul(param1, NULL, 0);

		if (threshold_time >= 0)
		{
			setThresholdTime(threshold_time);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 alg_param_threshold_freq_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threshold_freq = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threshold_freq = strtoul(param1, NULL, 0);

		if (threshold_freq >= 0)
		{
			setThresholdFreq(threshold_freq);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 alg_identify_UAV_cnt_th_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threshold_cnt = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threshold_cnt = strtoul(param1, NULL, 0);

		if (threshold_cnt >= 0)
		{
			setThresholdIdentifyUavCnt(threshold_cnt);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 alg_slide_win_len_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 slide_win_len = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		slide_win_len = strtoul(param1, NULL, 0);

		if (slide_win_len >= 0)
		{
			setSlideWinLen(slide_win_len);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 alg_warn_cnt_in_win_th_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 warn_cnt_in_win_th = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		warn_cnt_in_win_th = strtoul(param1, NULL, 0);

		if (warn_cnt_in_win_th >= 0)
		{
			setThresholdWarnCntInWin(warn_cnt_in_win_th);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static const CLI_Command_Definition_t alg_debug_printf_en =
	{
		"AlgDebugPrintfEn",
		"\r\n AlgDebugPrintfEn <enableSwitch>:\r\n enable algorithm debug printf(0:not, 1:yes)\r\n",
		alg_debug_printf_en_handler,
		1};

static const CLI_Command_Definition_t alg_identify_tc_match_en =
	{
		"AlgIdentifyTcMatchEn",
		"\r\n AlgIdentifyTcMatchEn <enableSwitch>:\r\n enable algorithm identify tuchuan match(0:not, 1:yes)\r\n",
		alg_identify_tc_match_en_handler,
		1};

static const CLI_Command_Definition_t alg_param_threshold_mag =
	{
		"AlgParamThresholdMagSet",
		"\r\n AlgParamThresholdMagSet <thresholdMag>:\r\n set algorithm parameter threshold mag(unit=0.001)\r\n",
		alg_param_threshold_mag_handler,
		1};

static const CLI_Command_Definition_t alg_param_threshold_time =
	{
		"AlgParamThresholdTimeSet",
		"\r\n AlgParamThresholdTimeSet <thresholdTime>:\r\n set algorithm parameter threshold time(unit=1)\r\n",
		alg_param_threshold_time_handler,
		1};

static const CLI_Command_Definition_t alg_param_threshold_freq =
	{
		"AlgParamThresholdFreqSet",
		"\r\n AlgParamThresholdFreqSet <thresholdFreq>:\r\n set algorithm parameter threshold freq(unit=1)\r\n",
		alg_param_threshold_freq_handler,
		1};

static const CLI_Command_Definition_t alg_identify_UAV_cnt_th =
	{
		"AlgIdentifyUavCntThresholdSet",
		"\r\n AlgIdentifyUavCntThresholdSet <thresholdCnt>:\r\n set algorithm identify UAV cnt threshold\r\n",
		alg_identify_UAV_cnt_th_handler,
		1};

static const CLI_Command_Definition_t alg_slide_win_len =
	{
		"AlgSlideWinLenSet",
		"\r\n AlgSlideWinLenSet <slideWinLen>:\r\n set algorithm slide window length\r\n",
		alg_slide_win_len_handler,
		1};

static const CLI_Command_Definition_t alg_warn_cnt_in_win_th =
	{
		"AlgWarnCntInWinThresholdSet",
		"\r\n AlgWarnCntInWinThresholdSet <thresholdWarnCnt>:\r\n set algorithm identify UAV warn count in slide window\r\n",
		alg_warn_cnt_in_win_th_handler,
		1};

void ut_alg_dbg_init(void)
{
	FreeRTOS_CLIRegisterCommand(&alg_debug_printf_en);
	FreeRTOS_CLIRegisterCommand(&alg_identify_tc_match_en);
	FreeRTOS_CLIRegisterCommand(&alg_param_threshold_mag);
	FreeRTOS_CLIRegisterCommand(&alg_param_threshold_time);
	FreeRTOS_CLIRegisterCommand(&alg_param_threshold_freq);
	FreeRTOS_CLIRegisterCommand(&alg_identify_UAV_cnt_th);
	FreeRTOS_CLIRegisterCommand(&alg_slide_win_len);
	FreeRTOS_CLIRegisterCommand(&alg_warn_cnt_in_win_th);
}
