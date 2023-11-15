/*
 * rf_config.h
 *
 *  Created on: 2022-09-20
 *      Author: A19199
 */
#ifndef RF_CONFIG_H
#define RF_CONFIG_H
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum RF_EVENT
{
	eRF_AD9361_CFG_F,
	eRF_PL_START_COLLECT,
	eRF_PL_STOP_COLLECT,
	eRF_AD9361_CFG_HIT_915,
	eRF_AD9361_CFG_HIT_GPS,
	eRF_AD9361_CFG_HIT_GPS_915,
	eRF_AD9361_CFG_CLOSE_HIT,
};

#define DAC_BUF_SIZE 512
extern uint32_t dac_cfg_B_2_4G[DAC_BUF_SIZE];
extern uint32_t dac_cfg_B_5_8G[DAC_BUF_SIZE];
extern uint32_t dac_cfg_A_5_2G[DAC_BUF_SIZE];
extern uint32_t dac_cfg_A_5_8G[DAC_BUF_SIZE];

void Rf_ConfigTask_Post(uint32_t flag);

void Rf_Ad9361_cfg_f(void);

int32_t InitRf_Config_Task();
uint8_t GetDetectFreqItem(void);
int32_t DacRegCfg_init();
int32_t DacVotlCfg_init();
void SetSweepFlag(uint8_t value);

#endif /* RF_CONFIG_H */
