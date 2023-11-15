
#include "ut_reg_set.h"

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
#include "../../hal/output/output.h"
#include "../../hal/smbus_driver/smbus_driver.h"
#include "../../hal/flash_hal/flash_hal.h"
#include "../../inc/common.h"
#include "../../srv/flash_info/cali_para_config.h"
#include "../../srv/log/log.h"

static BaseType_t get_frequency_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t set_frequency_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t get_battery_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t set_PA_KH_low_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t set_PA_KH_high_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t get_rf_log_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
//#define BATTERY_TEST

static const CLI_Command_Definition_t get_fre_command =
	{
		"getFreCmd",
		"\r\ngetFreCmd <register_ID>:\r\n Get single register value of frequency from flash\r\n",
		get_frequency_command_handler,
		1};

static const CLI_Command_Definition_t set_fre_command =
	{
		"setFreCmd",
		"\r\nsetFreCmd <register> <value>:\r\n Set register value of frequency\r\n",
		set_frequency_command_handler,
		2};

static const CLI_Command_Definition_t get_bat_command =
	{
		"getBatCapCmd",
		"\r\ngetBatCapCmd <register address>:\r\n Get battery register value\r\n",
		get_battery_command_handler,
		1};

static const CLI_Command_Definition_t set_PA_KH_low_command =
	{
		"setPakhLowCmd",
		"\r\nsetPakhLowCmd <register> <value>:\r\n Set register value of PA_KH_low\r\n",
		set_PA_KH_low_command_handler,
		2};

static const CLI_Command_Definition_t set_PA_KH_high_command =
	{
		"setPakhhighCmd",
		"\r\nsetPakhhighCmd <register> <value>:\r\n Set register value of PA_KH_high\r\n",
		set_PA_KH_high_command_handler,
		2};

static const CLI_Command_Definition_t get_RF_LOG_command =
	{
		"getRfLog",
		"\r\ngetRfLog  get Get the anti-gun log\r\n",
		get_rf_log_command_handler,
		1};

static BaseType_t get_frequency_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint16_t reg_num = 0;
	uint32_t reg_val = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		reg_num = strtoul(param1, NULL, 0);
		reg_val = getSglFreRegData((strick_fre_reg_id_t)reg_num);
		snprintf(pcWriteBuffer, xWriteBufferLen, "The register value is 0x%lx", reg_val);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t set_frequency_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	uint16_t reg_name = 0;
	uint32_t value = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		reg_name = strtoul(param1, NULL, 0);
		value = strtoul(param2, NULL, 0);
		write_register(reg_name, value);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t get_battery_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint8_t slave_addr = 0x16;
	uint8_t reg_addr = 0;
	uint8_t buff[2] = {0};
	uint8_t len = 2;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		reg_addr = strtoul(param1, NULL, 0);
		smbus_sw_read_register(slave_addr, reg_addr, buff, len);
		snprintf(pcWriteBuffer, xWriteBufferLen, "The register %d value is %x%x.", reg_addr, buff[1], buff[0]);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t set_PA_KH_low_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	uint32_t reg_name = 0;
	int32_t value = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		reg_name = strtoul(param1, NULL, 0);
		value = strtoul(param2, NULL, 0);
		axi_write_data(reg_name, value);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t set_PA_KH_high_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	uint32_t reg_name = 0;
	int32_t value = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		reg_name = strtoul(param1, NULL, 0);
		value = strtoul(param2, NULL, 0);
		axi_write_data(reg_name, value);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t get_rf_log_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	extern void LogUartPrint(u8 * pSendBuf, u32 len);

	BaseType_t len1 = 0;
	s32 value = 0;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		value = strtoul(param1, NULL, 0);
		if (1 == value)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "\n\r*****tinylog clear successfully!****\n");
		}
		else if (2 == value)
		{
			//		    tinyLog_selfTest();
			//		    tinyLog_refresh();
			//		    vTaskDelay( pdMS_TO_TICKS(10000)  );
			axi_write_data(0x0053, 1);
			snprintf(pcWriteBuffer, xWriteBufferLen, "\n\r*****log send finish!****\n");
		}
		else if (3 == value)
		{
			axi_write_data(0x0053, 0);
			SetSwitchOutputChl(0);
			snprintf(pcWriteBuffer, xWriteBufferLen, "\n\r*****SetSwitchOutputChl ps log****\n");
		}
		else if (3 == value)
		{
			axi_write_data(0x0053, 1);
			SetSwitchOutputChl(1);
			snprintf(pcWriteBuffer, xWriteBufferLen, "\n\r*****SetSwitchOutputChl pl log****\n");
		}
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

#ifdef BATTERY_TEST
void Smbus_Task(void *p_arg)
{
	uint16_t capacity;
	while (1)
	{
		get_batteryRemainCaphandler(&capacity);
		GetRelativeStateOfCharge(&capacity);
		vTaskDelay(1500);
	}
}
#endif

int ut_set_reg_init(void)
{
	int ret = RET_OK;
	uint8_t slave_addr = 0x16;
	uint8_t reg_addr = 0x0d;
	uint8_t buff[2] = {0};
	uint8_t len = 1;

	FreeRTOS_CLIRegisterCommand(&get_fre_command);
	FreeRTOS_CLIRegisterCommand(&set_fre_command);
	FreeRTOS_CLIRegisterCommand(&get_bat_command);
	FreeRTOS_CLIRegisterCommand(&set_PA_KH_low_command);
	FreeRTOS_CLIRegisterCommand(&set_PA_KH_high_command);
	FreeRTOS_CLIRegisterCommand(&get_RF_LOG_command);

	//	ret = smbus_sw_read_register(slave_addr, reg_addr, buff, len);
	//	if(ret) //battery not plugged in
	//	{
	//		axi_write_data(0x0053 , 1 ) ;
	//		tinyLog_stop();
	//	}

#ifdef BATTERY_TEST
	if (pdPASS != xTaskCreate(Smbus_Task, "Smbus_Task", 256U, NULL, (configMAX_PRIORITIES - 1), NULL))
	{
		ret = RET_SYSTEM_ERR;
	}
#endif
	return ret;
}
