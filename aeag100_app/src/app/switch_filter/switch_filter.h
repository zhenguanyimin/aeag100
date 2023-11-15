#include <stdint.h>

enum RF5250_CHANNEL
{
	eRF5250_CLOSE,
	eRF5250_CHANNEL1,
	eRF5250_CHANNEL2,
	eRF5250_CHANNEL3,
	eRF5250_CHANNEL4,
	eRF5250_CHANNEL5,
};

enum HMC1119_SERNIN
{
	eHMC1119_SERNIN0, // attenuation 0 db
	eHMC1119_SERNIN1, // attenuation 0.25
	eHMC1119_SERNIN2, // attenuation 0.5
	eHMC1119_SERNIN3, // attenuation 1
	eHMC1119_SERNIN4, // attenuation 2
	eHMC1119_SERNIN5, // attenuation 4
	eHMC1119_SERNIN6, // attenuation 8
	eHMC1119_SERNIN7, // attenuation  16
	eHMC1119_SERNIN8, // attenuation 31.75
};

int32_t SimulaAdp_Init();

void SimulaAdp_SendSerninCmd(uint8_t cmdNum);

void SimulaAdp_SendAdrf5250md(uint8_t cmdNum);

void SimulaAdp_SendData(uint8_t *buff, uint32_t len);
