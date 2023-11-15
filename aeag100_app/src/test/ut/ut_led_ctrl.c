/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "ut_led_ctrl.h"

#include "../../app/logging/logging.h"
#include "../../app/process_task/hit_process_task.h"
#include "../../app/rf_config/rf_config.h"
#include "../../drv/flash/flash.h"
#include "../../hal/flash_hal/flash_hal.h"
#include "../../hal/hal.h"
#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../srv/log/log.h"
#include "../../cfg/aeag_config.h"
#include "../../srv/flash_info/cali_para_config.h"

s32 ut_led_ctrl_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		HitProcessTask_Post(type);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 ad_start_enable_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		if (1 == type)
		{
			axi_write_data(0x01, 1);
		}
		else if (0 == type)
		{
			axi_write_data(0x01, 0);
		}

		if ((1 == type) || (0 == type))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

s32 ad9361_cfg_f_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		Rf_ConfigTask_Post(type);
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

s32 ADRF5250_RF_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SimulaAdp_SendAdrf5250md(type);
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

s32 SERNIN_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SimulaAdp_SendSerninCmd(type);
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

s32 enable_ad936x_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetAD9361Gpio(type);
		//		if (1==type)
		//		{
		////			enable_ad936x();
		////			InitAd9361();
		//		}
		//		else if (0==type)
		//		{
		////			disable_ad936x();
		//		}
		//
		//		if ((1==type)||(0==type))
		//		{
		//			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		//		}else
		//			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

static BaseType_t setDacVolt(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint32_t type[12];

	for (uint8_t i = 0; i < 12; i++)
	{
		type[i] = strtoul(FreeRTOS_CLIGetParameter(pcCommandString, i + 1, &len1), NULL, 0);
	}
	//	if ((len1 > 0) && (len2 > 0)
	//			&& (len3 > 0) && (len4 > 0)
	//			&& (len5 > 0) && (len6 > 0)
	//			 && (len7 > 0) && (len8 > 0))
	{
		FlashHal_Erase(0xA00000, 4);
		for (uint8_t i = 0; i < 12; i++)
		{
			setSinglFreRegData(dataDacVolt1 + i, type[i]);
		}
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		DacVotlCfg_init();
	}
	//	else
	//	{
	//		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	//	}

	return pdFALSE;
}

static BaseType_t DacVotlCfg_init_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	DacVotlCfg_init();

	return pdFALSE;
}

s32 SetSweepFlag_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetSweepFlag(type);
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

s32 ut_SetAd9361Gain_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetAd9361Gain(type);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 ut_SetAd9361GainAGC_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetAd9361GainAGC((uint8_t)type);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 ut_SetAd9361GainCutover_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0, len3 = 0;
	s32 type = -1, data1, data2;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);

	if ((len1 > 0) && (len2 > 0) && (len3 > 0))
	{
		type = strtoul(param1, NULL, 0);
		data1 = strtoul(param2, NULL, 0);
		data2 = strtoul(param3, NULL, 0);
		SetAd9361GainCutoverFlag((uint8_t)type);
		SetAd9361GainCutoverValue((uint8_t)data1, (uint8_t)data2);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

s32 SetSwitchOutputChl_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetSwitchOutputChl(type);
		if ((1 == type) || (0 == type))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

s32 SetOutputSpecMat_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	sLogging_msg_t msg;
	memset(msg.buf, 0x56, 30);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetOutputSpecMat(type);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

s32 LoggingMsg_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	sLogging_msg_t msg;
	memset(msg.buf, 0x56, 30);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		if (type == 0)
		{
			ClearLogging();
		}
		else if (type == 1)
		{
			axi_write_data(0x0053, 1);
			msg.uLogNum = eLOG_LOGGING_OUTPUT;
			LoggingTask_Post(&msg);
		}
		else if (type == 2)
		{
			SetSwitchOutputChl(0);
		}
		else if (type == 3)
		{
			axi_write_data(0x0053, 1);
			SetSwitchOutputChl(2);
		}
		else if (type == 4)
		{
			axi_write_data(0x0053, 1);
			SetSwitchOutputChl(1);
		}
		else if (type == 5)
		{
			LoggingTest();
		}
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

s32 Version_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	uint32_t PL_Version_date;
	uint32_t PL_Version_time;

	PL_Version_date = axi_read_data(0x09);
	PL_Version_time = axi_read_data(0x0A);
	LOG_DEBUG(" Firmware compile time:%s %s\n", __DATE__, __TIME__);
	LOG_DEBUG("version:%s\r\n", EMBED_SOFTWARE_PS_VERSION_STR);
	LOG_DEBUG("PL_Version: %X %X\r\n", PL_Version_date & 0xFFFF, PL_Version_time);
	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	return pdFALSE;
}

static const CLI_Command_Definition_t SetSweepFlag_cmd =
	{
		"SetSweepFlag",
		"\r\nSetSweepFlag <type>:\r\n type:0 for disable ,1 for enable \r\n",
		SetSweepFlag_handler,
		1};

static const CLI_Command_Definition_t DacVotlCfg_init_cmd =
	{
		"DacVotlCfg_init",
		"\r\nDacVotlCfg_init\r\n",
		DacVotlCfg_init_handler,
		0};

static const CLI_Command_Definition_t setDacVolt_cmd =
	{
		"setDacVolt",
		"\r\nsetDacVolt <Votage_start1> <Votage_end1> <Votage_start1> <Votage_end1> <Votage_start1> <Votage_end1> <Votage_start1> <Votage_end1>:\r\n type:0  \r\n",
		setDacVolt,
		12};

static const CLI_Command_Definition_t ad9361_cfg_f =
	{
		"ad9361_cfg_f",
		"\r\nad9361_cfg_f :\r\n",
		ad9361_cfg_f_handler,
		1};

static const CLI_Command_Definition_t enable_ad936x_f =
	{
		"enable_ad936x",
		"\r\nenable_ad936x <type>:\r\n type:0 for disable ,1 for enable \r\n",
		enable_ad936x_handler,
		1};

static const CLI_Command_Definition_t ADRF5250_RF_enable =
	{
		"ADRF5250_RF",
		"\r\nADRF5250_RF <type>: 1-5 RF channel\r\n",
		ADRF5250_RF_handler,
		1};

static const CLI_Command_Definition_t SERNIN_enable =
	{
		"SERNIN",
		"\r\nSERNIN <type>: 0-8 Attenuation assignment\r\n",
		SERNIN_handler,
		1};

static const CLI_Command_Definition_t ut_led_ctrl =
	{
		"HitProcessTask",
		"\r\nHitProcessTask <type>: MANUAL_HIT = 0x02,CLOSE_HIT = 0x03,WARNING_LEVAL1 = 0x06,WARNING_LEVAL2 = 0x07,WARNING_LEVAL3 = 0x08,NO_WARNING\r\n",
		ut_led_ctrl_handler,
		1};

static const CLI_Command_Definition_t ut_SetAd9361Gain_ctrl =
	{
		"SetAd9361Gain",
		"\r\nSetAd9361Gain <value>: Ad9361 Gain\r\n",
		ut_SetAd9361Gain_handler,
		1};

static const CLI_Command_Definition_t ut_SetAd9361GainAGC_ctrl =
	{
		"SetAd9361GainAGC",
		"\r\nSetAd9361GainAGC <value>: Ad9361 Gain\r\n",
		ut_SetAd9361GainAGC_handler,
		1};

static const CLI_Command_Definition_t ut_SetAd9361GainCutover_ctrl =
	{
		"SetAd9361GainCutover",
		"\r\nSetAd9361GainCutover <mode> <value1> <value2>: <mode> 1 open 0 close  <value1> Gain <value2> Gain\r\n",
		ut_SetAd9361GainCutover_handler,
		3};

static const CLI_Command_Definition_t SetSwitchOutputChl_cmd =
	{
		"SetSwitchOutputChl",
		"\r\nSetSwitchOutputChl <type>:\r\n type:0 ps printf ,1 pl log printf \r\n",
		SetSwitchOutputChl_handler,
		1};

static const CLI_Command_Definition_t LoggingMsg_cmd =
	{
		"LoggingMsg",
		"\r\nLoggingMsg <type>:0:Clear Logging  1:logging output 2:ps output debug 3:pl uart2 output debug\r\n  \r\n",
		LoggingMsg_handler,
		1};

static const CLI_Command_Definition_t SetOutputSpecMat_cmd =
	{
		"SetOutputSpecMat",
		"\r\nSetOutputSpecMat <type>:0:disable  1:enable\r\n  \r\n",
		SetOutputSpecMat_handler,
		1};

static const CLI_Command_Definition_t Version_cmd =
	{
		"Version",
		"\r\nVersion\r\n  \r\n",
		Version_handler,
		0};

void ut_led_ctrl_init(void)
{
	FreeRTOS_CLIRegisterCommand(&SetSweepFlag_cmd);
	FreeRTOS_CLIRegisterCommand(&ut_led_ctrl);
	FreeRTOS_CLIRegisterCommand(&ad9361_cfg_f);
	FreeRTOS_CLIRegisterCommand(&ADRF5250_RF_enable);
	FreeRTOS_CLIRegisterCommand(&SERNIN_enable);
	FreeRTOS_CLIRegisterCommand(&enable_ad936x_f);
	FreeRTOS_CLIRegisterCommand(&setDacVolt_cmd);
	FreeRTOS_CLIRegisterCommand(&ut_SetAd9361Gain_ctrl);
	FreeRTOS_CLIRegisterCommand(&ut_SetAd9361GainAGC_ctrl);
	FreeRTOS_CLIRegisterCommand(&ut_SetAd9361GainCutover_ctrl);
	FreeRTOS_CLIRegisterCommand(&SetSwitchOutputChl_cmd);
	FreeRTOS_CLIRegisterCommand(&LoggingMsg_cmd);
	FreeRTOS_CLIRegisterCommand(&SetOutputSpecMat_cmd);
	FreeRTOS_CLIRegisterCommand(&DacVotlCfg_init_cmd);
	FreeRTOS_CLIRegisterCommand(&Version_cmd);
}
