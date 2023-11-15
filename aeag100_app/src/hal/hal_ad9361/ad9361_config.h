#ifndef _AD9361_CONFIG_H_
#define _AD9361_CONFIG_H_
#include <sys/_stdint.h>

//#define PROJECT_VER "HJX MINI-TRX-Z-MGC-Rx-Gain Index-76"   //"HJX MINI-TRX-Z-TEST"
#define PROJECT_VER "HJX MINI-TRX-Z-2020 SDR"


#define AD9009_VERSION_X "2"
#define AD9009_VERSION_Y "0"
#define AD9009_VERSION_Z "0" 

//#define  Freq_MAX_CNT 10

enum Freq_CFG
{
//	Freq_RX_433m,
//	Freq_RX_842m,
//	Freq_RX_915m,
	Freq_RX_2420m,
//	Freq_RX_837m,
//	Freq_RX_2406m,
	Freq_RX_2460m,
//	Freq_RX_5160m,
//	Freq_RX_5200m,
//	Freq_RX_5240m,
	Freq_RX_5745m,
	Freq_RX_5785m,
	Freq_RX_5825m,
	Freq_RX_5865m,

//	Freq_RX_875m,
//	Freq_RX_2417m,
//	Freq_RX_2447m,
//	Freq_RX_2477m,
//	Freq_RX_5157m,
//	Freq_RX_5187m,
//	Freq_RX_5217m,
//	Freq_RX_5247m,
//	Freq_RX_5742m,
//	Freq_RX_5772m,
//	Freq_RX_5802m,
//	Freq_RX_5832m,
//	Freq_RX_5862m,
	Freq_MAX_CNT,
};

enum Freq_TX_CFG
{
	Freq_TX_915m= 0x01,
	Freq_TX_GPS,
	Freq_TX_GPS_915m,
};

uint32_t get_rx_freq(uint32_t freqItem);
uint32_t set_rx_freq(uint32_t freq);
void cfgAd9361(void);
int32_t InitAd9361(void);
int32_t InitAd9361_Rx(void);
void ad9361_cfg_tx_init();
int32_t Ad9361_fre_hop_init(void);
int32_t Ad9361_fre_hop_set(int32_t rfRang);
int32_t Ad9361_fre_hop_start(void);
int32_t Ad9361_fre_hop_stop(void);
void SetAD9361Gpio(uint32_t value);
void SetAd9361Gain(uint8_t value);
void SetAd9361GainCutoverFlag(uint8_t value);
uint8_t GetAd9361GainCutoverFlag(void);
void SetAd9361GainCutoverValue(uint8_t value1, uint8_t value2);
#endif

