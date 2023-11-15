#include "ad9361_config.h"
#include "zynq_bram.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xparameters.h"

#include "../../srv/log/log.h"
#define AD9361_FRE_HOP_TIME (85)
static XGpio sGpio = {0};
extern XScuGic xInterruptController;
static uint8_t uGps915mHitFlag = Freq_TX_GPS_915m;

static uint8_t Ad9361GainBuf[5] = {0x32, 0x28, 0x00, 0x00, 0x00};

enum Freq_CFG_ADDR
{
	/*915*/
	fre_433m_addr = 0x00,
	//	fre_1931m_addr = 0x00,
	fre_1433m_addr,
	fre_842m_addr,
	fre_915m_addr,

	/*GPS*/
	//	fre_1187m_addr,
	//	fre_1212m_addr,
	//	fre_1253m_addr,
	//	fre_1265m_addr,
	fre_1572m_addr,
	fre_1597m_addr,
	fre_max,
};

uint8_t reg_value_list_433m[17] = {0x30, 0x56, 0x0, 0x66, 0xc4, 0x4c, 0x5b, 0x32, 0x69, 0x82, 0x8f, 0xf6, 0xd5, 0xe, 0x17, 0x70, 0xd};
uint8_t reg_value_list_1931m[17] = {0x11, 0x60, 0x00, 0x5E, 0x66, 0x46, 0x4F, 0xCE, 0x60, 0xC1, 0x90, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0C};
uint8_t reg_value_list_842m[17] = {0x20, 0x54, 0x00, 0x50, 0x97, 0x19, 0x5b, 0x32, 0x69, 0x82, 0x8f, 0xf6, 0xd5, 0xe, 0x17, 0x70, 0xd};
uint8_t reg_value_list_915m[17] = {0x20, 0x5b, 0x0, 0x7f, 0xfd, 0x3f, 0x58, 0xf0, 0x70, 0x82, 0x8e, 0xf6, 0xd5, 0xe, 0x1f, 0x70, 0xd};
uint8_t reg_value_list_1187m[17] = {0x20, 0x76, 0x0, 0x5a, 0x96, 0x59, 0x58, 0x5e, 0x60, 0x81, 0x8c, 0xf6, 0xd5, 0xe, 0x16, 0x70, 0xc};
uint8_t reg_value_list_1212m[17] = {0x20, 0x79, 0x0, 0x50, 0x96, 0x19, 0x53, 0x55, 0x68, 0x81, 0x8b, 0xf6, 0xd5, 0xe, 0x16, 0x70, 0xc};
uint8_t reg_value_list_1253m[17] = {0x20, 0x7d, 0x0, 0xff, 0x62, 0x26, 0x50, 0x48, 0x70, 0x81, 0x8b, 0xf6, 0xd5, 0xe, 0x16, 0x70, 0xc};
uint8_t reg_value_list_1265m[17] = {0x20, 0x7e, 0x0, 0x8d, 0xfc, 0x3f, 0x50, 0x44, 0x78, 0x81, 0x8a, 0xf6, 0xd5, 0xe, 0x16, 0x70, 0xc};
uint8_t reg_value_list_1433m[17] = {0x20, 0x8F, 0x00, 0x62, 0x66, 0x26, 0x54, 0x19, 0x78, 0xC1, 0x88, 0xF6, 0xD5, 0x0E, 0x14, 0x70, 0x0C};
uint8_t reg_value_list_1572m[17] = {0x10, 0x4e, 0x0, 0xa4, 0xca, 0x4c, 0x51, 0x72, 0x79, 0x82, 0x8f, 0xf6, 0xd5, 0xe, 0x1f, 0x70, 0xd};
uint8_t reg_value_list_1597m[17] = {0x10, 0x4f, 0x0, 0x97, 0xca, 0x6c, 0x50, 0x63, 0x79, 0x82, 0x8f, 0xf6, 0xd5, 0xe, 0x1f, 0x70, 0xd};
uint16_t TX_read_reg_addr_list[17] = {0x005, 0x271, 0x272, 0x273, 0x274, 0x275, 0x276, 0x277, 0x278, 0x279, 0x27B, 0x27E, 0x27F, 0x280, 0x282, 0x290, 0x291};

void Ad9361Config_01(void)
{
	u16 row, i;
	u8 Ad9361Config[][3] = {
		{0x80, 0x00, 0x81},
		{0x80, 0x00, 0x00},
		//************************************************************
		// AD9361 R2 Auto Generated Initialization Script:  This script was
		// generated using the AD9361 Customer software Version 2.1.3
		//************************************************************
		// Profile: Custom
		// REFCLK_IN: 40.000 MHz

		// RESET_FPGA
		// RESET_DUT

		// BlockWrite	2,6	// Set ADI FPGA SPI to 20Mhz
		{0x83, 0xDF, 0x01}, // SPIWrite	3DF,01	// Required for proper operation
		// ReadPartNumber
		{0x82, 0xA6, 0x0E}, // SPIWrite	2A6,0E	// Enable Master Bias
		{0x82, 0xA8, 0x0E}, // SPIWrite	2A8,0E	// Set Bandgap Trim
		// REFCLK_Scale	40.000000,1,2	// Sets local variables in script engine, user can ignore
		{0x82, 0xAB, 0x07}, // SPIWrite	2AB,07	// Set RF PLL reflclk scale to REFCLK * 2
		{0x82, 0xAC, 0xFF}, // SPIWrite	2AC,FF	// Set RF PLL reflclk scale to REFCLK * 2
		{0x80, 0x09, 0x17}, // SPIWrite	009,17	// Enable Clocks
		{0xF0, 0x00, 0x14}, // WAIT	20	// waits 20 ms

		//************************************************************
		// Set BBPLL Frequency: 1280.000000
		//************************************************************
		{0x80, 0x45, 0x00}, // SPIWrite	045,00	// Set BBPLL reflclk scale to REFCLK /1
		{0x80, 0x46, 0x05}, // SPIWrite	046,05	// Set BBPLL Loop Filter Charge Pump current
		{0x80, 0x48, 0xE8}, // SPIWrite	048,E8	// Set BBPLL Loop Filter C1, R1
		{0x80, 0x49, 0x5B}, // SPIWrite	049,5B	// Set BBPLL Loop Filter R2, C2, C1
		{0x80, 0x4A, 0x35}, // SPIWrite	04A,35	// Set BBPLL Loop Filter C3,R2
		{0x80, 0x4B, 0xE0}, // SPIWrite	04B,E0	// Allow calibration to occur and set cal count to 1024 for max accuracy
		{0x80, 0x4E, 0x10}, // SPIWrite	04E,10	// Set calibration clock to REFCLK/4 for more accuracy
		{0x80, 0x43, 0x00}, // SPIWrite	043,00	// BBPLL Freq Word (Fractional[7:0])
		{0x80, 0x42, 0x00}, // SPIWrite	042,00	// BBPLL Freq Word (Fractional[15:8])
		{0x80, 0x41, 0x00}, // SPIWrite	041,00	// BBPLL Freq Word (Fractional[23:16])
		{0x80, 0x44, 0x20}, // SPIWrite	044,20	// BBPLL Freq Word (Integer[7:0])
		{0x80, 0x3F, 0x05}, // SPIWrite	03F,05	// Start BBPLL Calibration
		{0x80, 0x3F, 0x01}, // SPIWrite	03F,01	// Clear BBPLL start calibration bit
		{0x80, 0x4C, 0x86}, // SPIWrite	04C,86	// Increase BBPLL KV and phase margin
		{0x80, 0x4D, 0x01}, // SPIWrite	04D,01	// Increase BBPLL KV and phase margin
		{0x80, 0x4D, 0x05}, // SPIWrite	04D,05	// Increase BBPLL KV and phase margin
		{0x00, 0x5E, 0x71}, // WAIT_CALDONE	BBPLL,2000	// Wait for BBPLL to lock, Timeout 2sec, Max BBPLL VCO Cal Time: 345.600 us (Done when 0x05E[7]==1)

		{0x80, 0x02, 0xDD}, // SPIWrite	002,DD	// Setup Tx Digital Filters/ Channels
		{0x80, 0x03, 0xDD}, // SPIWrite	003,DD	// Setup Rx Digital Filters/ Channels
		{0x80, 0x04, 0x03}, // SPIWrite	004,03	// Select Rx input pin(A,B,C)/ Tx out pin (A,B)
		{0x80, 0x0A, 0x02}, // SPIWrite	00A,02	// Set BBPLL post divide rate

		//************************************************************
		// Program Tx FIR: C:\Program Files (x86)\Analog Devices\AD9361R2
		// Evaluation Software 2.1.3\DigitalFilters\0905-40-64(2).ftr
		//************************************************************
		{0x80, 0x65, 0x9A}, // SPIWrite	065,9A	// Enable clock to Tx FIR Filter and set Filter gain Setting
		{0x80, 0x60, 0x00}, // SPIWrite	060,00	// Write FIR coefficient address
		{0x80, 0x61, 0x05}, // SPIWrite	061,05	// Write FIR coefficient data[7:0]
		{0x80, 0x62, 0x00}, // SPIWrite	062,00	// Write FIR coefficient data[15:8]
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E	// Set Write EN to push data into FIR filter register map
		{0x80, 0x64, 0x00}, // SPIWrite	064,00	// Write to Read only register to delay ~1us
		{0x80, 0x64, 0x00}, // SPIWrite	064,00	// Write to Read only register to delay ~1us
		{0x80, 0x60, 0x01}, // SPIWrite	060,01
		{0x80, 0x61, 0xBF}, // SPIWrite	061,BF
		{0x80, 0x62, 0x06}, // SPIWrite	062,06
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x02}, // SPIWrite	060,02
		{0x80, 0x61, 0xFF}, // SPIWrite	061,FF
		{0x80, 0x62, 0xFE}, // SPIWrite	062,FE
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x03}, // SPIWrite	060,03
		{0x80, 0x61, 0xF4}, // SPIWrite	061,F4
		{0x80, 0x62, 0xFF}, // SPIWrite	062,FF
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x04}, // SPIWrite	060,04
		{0x80, 0x61, 0xED}, // SPIWrite	061,ED
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x05}, // SPIWrite	060,05
		{0x80, 0x61, 0x57}, // SPIWrite	061,57
		{0x80, 0x62, 0xFE}, // SPIWrite	062,FE
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x06}, // SPIWrite	060,06
		{0x80, 0x61, 0x3C}, // SPIWrite	061,3C
		{0x80, 0x62, 0x02}, // SPIWrite	062,02
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x07}, // SPIWrite	060,07
		{0x80, 0x61, 0x5B}, // SPIWrite	061,5B
		{0x80, 0x62, 0xFD}, // SPIWrite	062,FD
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x08}, // SPIWrite	060,08
		{0x80, 0x61, 0xE6}, // SPIWrite	061,E6
		{0x80, 0x62, 0x02}, // SPIWrite	062,02
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x09}, // SPIWrite	060,09
		{0x80, 0x61, 0x06}, // SPIWrite	061,06
		{0x80, 0x62, 0xFD}, // SPIWrite	062,FD
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x0A}, // SPIWrite	060,0A
		{0x80, 0x61, 0xDB}, // SPIWrite	061,DB
		{0x80, 0x62, 0x02}, // SPIWrite	062,02
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x0B}, // SPIWrite	060,0B
		{0x80, 0x61, 0x7D}, // SPIWrite	061,7D
		{0x80, 0x62, 0xFD}, // SPIWrite	062,FD
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x0C}, // SPIWrite	060,0C
		{0x80, 0x61, 0xF1}, // SPIWrite	061,F1
		{0x80, 0x62, 0x01}, // SPIWrite	062,01
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x0D}, // SPIWrite	060,0D
		{0x80, 0x61, 0xDC}, // SPIWrite	061,DC
		{0x80, 0x62, 0xFE}, // SPIWrite	062,FE
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x0E}, // SPIWrite	060,0E
		{0x80, 0x61, 0x1E}, // SPIWrite	061,1E
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x0F}, // SPIWrite	060,0F
		{0x80, 0x61, 0x10}, // SPIWrite	061,10
		{0x80, 0x62, 0x01}, // SPIWrite	062,01
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x10}, // SPIWrite	060,10
		{0x80, 0x61, 0xA4}, // SPIWrite	061,A4
		{0x80, 0x62, 0xFD}, // SPIWrite	062,FD
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x11}, // SPIWrite	060,11
		{0x80, 0x61, 0xB1}, // SPIWrite	061,B1
		{0x80, 0x62, 0x03}, // SPIWrite	062,03
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x12}, // SPIWrite	060,12
		{0x80, 0x61, 0x0A}, // SPIWrite	061,0A
		{0x80, 0x62, 0xFB}, // SPIWrite	062,FB
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x13}, // SPIWrite	060,13
		{0x80, 0x61, 0x10}, // SPIWrite	061,10
		{0x80, 0x62, 0x06}, // SPIWrite	062,06
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x14}, // SPIWrite	060,14
		{0x80, 0x61, 0x1F}, // SPIWrite	061,1F
		{0x80, 0x62, 0xF9}, // SPIWrite	062,F9
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x15}, // SPIWrite	060,15
		{0x80, 0x61, 0x49}, // SPIWrite	061,49
		{0x80, 0x62, 0x07}, // SPIWrite	062,07
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x16}, // SPIWrite	060,16
		{0x80, 0x61, 0xD8}, // SPIWrite	061,D8
		{0x80, 0x62, 0xF8}, // SPIWrite	062,F8
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x17}, // SPIWrite	060,17
		{0x80, 0x61, 0x5F}, // SPIWrite	061,5F
		{0x80, 0x62, 0x06}, // SPIWrite	062,06
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x18}, // SPIWrite	060,18
		{0x80, 0x61, 0x33}, // SPIWrite	061,33
		{0x80, 0x62, 0xFB}, // SPIWrite	062,FB
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x19}, // SPIWrite	060,19
		{0x80, 0x61, 0x52}, // SPIWrite	061,52
		{0x80, 0x62, 0x02}, // SPIWrite	062,02
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x1A}, // SPIWrite	060,1A
		{0x80, 0x61, 0x33}, // SPIWrite	061,33
		{0x80, 0x62, 0x01}, // SPIWrite	062,01
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x1B}, // SPIWrite	060,1B
		{0x80, 0x61, 0x1B}, // SPIWrite	061,1B
		{0x80, 0x62, 0xFA}, // SPIWrite	062,FA
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x1C}, // SPIWrite	060,1C
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x0C}, // SPIWrite	062,0C
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x1D}, // SPIWrite	060,1D
		{0x80, 0x61, 0x8C}, // SPIWrite	061,8C
		{0x80, 0x62, 0xEC}, // SPIWrite	062,EC
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x1E}, // SPIWrite	060,1E
		{0x80, 0x61, 0x60}, // SPIWrite	061,60
		{0x80, 0x62, 0x1C}, // SPIWrite	062,1C
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x1F}, // SPIWrite	060,1F
		{0x80, 0x61, 0x9C}, // SPIWrite	061,9C
		{0x80, 0x62, 0xD6}, // SPIWrite	062,D6
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x20}, // SPIWrite	060,20
		{0x80, 0x61, 0x7A}, // SPIWrite	061,7A
		{0x80, 0x62, 0x59}, // SPIWrite	062,59
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x21}, // SPIWrite	060,21
		{0x80, 0x61, 0x7A}, // SPIWrite	061,7A
		{0x80, 0x62, 0x59}, // SPIWrite	062,59
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x22}, // SPIWrite	060,22
		{0x80, 0x61, 0x9C}, // SPIWrite	061,9C
		{0x80, 0x62, 0xD6}, // SPIWrite	062,D6
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x23}, // SPIWrite	060,23
		{0x80, 0x61, 0x60}, // SPIWrite	061,60
		{0x80, 0x62, 0x1C}, // SPIWrite	062,1C
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x24}, // SPIWrite	060,24
		{0x80, 0x61, 0x8C}, // SPIWrite	061,8C
		{0x80, 0x62, 0xEC}, // SPIWrite	062,EC
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x25}, // SPIWrite	060,25
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x0C}, // SPIWrite	062,0C
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x26}, // SPIWrite	060,26
		{0x80, 0x61, 0x1B}, // SPIWrite	061,1B
		{0x80, 0x62, 0xFA}, // SPIWrite	062,FA
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x27}, // SPIWrite	060,27
		{0x80, 0x61, 0x33}, // SPIWrite	061,33
		{0x80, 0x62, 0x01}, // SPIWrite	062,01
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x28}, // SPIWrite	060,28
		{0x80, 0x61, 0x52}, // SPIWrite	061,52
		{0x80, 0x62, 0x02}, // SPIWrite	062,02
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x29}, // SPIWrite	060,29
		{0x80, 0x61, 0x33}, // SPIWrite	061,33
		{0x80, 0x62, 0xFB}, // SPIWrite	062,FB
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x2A}, // SPIWrite	060,2A
		{0x80, 0x61, 0x5F}, // SPIWrite	061,5F
		{0x80, 0x62, 0x06}, // SPIWrite	062,06
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x2B}, // SPIWrite	060,2B
		{0x80, 0x61, 0xD8}, // SPIWrite	061,D8
		{0x80, 0x62, 0xF8}, // SPIWrite	062,F8
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x2C}, // SPIWrite	060,2C
		{0x80, 0x61, 0x49}, // SPIWrite	061,49
		{0x80, 0x62, 0x07}, // SPIWrite	062,07
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x2D}, // SPIWrite	060,2D
		{0x80, 0x61, 0x1F}, // SPIWrite	061,1F
		{0x80, 0x62, 0xF9}, // SPIWrite	062,F9
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x2E}, // SPIWrite	060,2E
		{0x80, 0x61, 0x10}, // SPIWrite	061,10
		{0x80, 0x62, 0x06}, // SPIWrite	062,06
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x2F}, // SPIWrite	060,2F
		{0x80, 0x61, 0x0A}, // SPIWrite	061,0A
		{0x80, 0x62, 0xFB}, // SPIWrite	062,FB
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x30}, // SPIWrite	060,30
		{0x80, 0x61, 0xB1}, // SPIWrite	061,B1
		{0x80, 0x62, 0x03}, // SPIWrite	062,03
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x31}, // SPIWrite	060,31
		{0x80, 0x61, 0xA4}, // SPIWrite	061,A4
		{0x80, 0x62, 0xFD}, // SPIWrite	062,FD
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x32}, // SPIWrite	060,32
		{0x80, 0x61, 0x10}, // SPIWrite	061,10
		{0x80, 0x62, 0x01}, // SPIWrite	062,01
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x33}, // SPIWrite	060,33
		{0x80, 0x61, 0x1E}, // SPIWrite	061,1E
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x34}, // SPIWrite	060,34
		{0x80, 0x61, 0xDC}, // SPIWrite	061,DC
		{0x80, 0x62, 0xFE}, // SPIWrite	062,FE
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x35}, // SPIWrite	060,35
		{0x80, 0x61, 0xF1}, // SPIWrite	061,F1
		{0x80, 0x62, 0x01}, // SPIWrite	062,01
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x36}, // SPIWrite	060,36
		{0x80, 0x61, 0x7D}, // SPIWrite	061,7D
		{0x80, 0x62, 0xFD}, // SPIWrite	062,FD
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x37}, // SPIWrite	060,37
		{0x80, 0x61, 0xDB}, // SPIWrite	061,DB
		{0x80, 0x62, 0x02}, // SPIWrite	062,02
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x38}, // SPIWrite	060,38
		{0x80, 0x61, 0x06}, // SPIWrite	061,06
		{0x80, 0x62, 0xFD}, // SPIWrite	062,FD
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x39}, // SPIWrite	060,39
		{0x80, 0x61, 0xE6}, // SPIWrite	061,E6
		{0x80, 0x62, 0x02}, // SPIWrite	062,02
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x3A}, // SPIWrite	060,3A
		{0x80, 0x61, 0x5B}, // SPIWrite	061,5B
		{0x80, 0x62, 0xFD}, // SPIWrite	062,FD
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x3B}, // SPIWrite	060,3B
		{0x80, 0x61, 0x3C}, // SPIWrite	061,3C
		{0x80, 0x62, 0x02}, // SPIWrite	062,02
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x3C}, // SPIWrite	060,3C
		{0x80, 0x61, 0x57}, // SPIWrite	061,57
		{0x80, 0x62, 0xFE}, // SPIWrite	062,FE
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x3D}, // SPIWrite	060,3D
		{0x80, 0x61, 0xED}, // SPIWrite	061,ED
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x3E}, // SPIWrite	060,3E
		{0x80, 0x61, 0xF4}, // SPIWrite	061,F4
		{0x80, 0x62, 0xFF}, // SPIWrite	062,FF
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x3F}, // SPIWrite	060,3F
		{0x80, 0x61, 0xFF}, // SPIWrite	061,FF
		{0x80, 0x62, 0xFE}, // SPIWrite	062,FE
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x40}, // SPIWrite	060,40
		{0x80, 0x61, 0xBF}, // SPIWrite	061,BF
		{0x80, 0x62, 0x06}, // SPIWrite	062,06
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x41}, // SPIWrite	060,41
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x42}, // SPIWrite	060,42
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x43}, // SPIWrite	060,43
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x44}, // SPIWrite	060,44
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x45}, // SPIWrite	060,45
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x46}, // SPIWrite	060,46
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x47}, // SPIWrite	060,47
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x48}, // SPIWrite	060,48
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x49}, // SPIWrite	060,49
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x4A}, // SPIWrite	060,4A
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
	};
	row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
	for (i = 0; i < row; i++)
	{
		SetAd(Ad9361Config[i]);
	}
}

void Ad9361Config_02(void)
{
	u16 row, i;
	u8 Ad9361Config[][3] = {
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x4B}, // SPIWrite	060,4B
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x4C}, // SPIWrite	060,4C
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x4D}, // SPIWrite	060,4D
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x4E}, // SPIWrite	060,4E
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x60, 0x4F}, // SPIWrite	060,4F
		{0x80, 0x61, 0x00}, // SPIWrite	061,00
		{0x80, 0x62, 0x00}, // SPIWrite	062,00
		{0x80, 0x65, 0x9E}, // SPIWrite	065,9E
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x64, 0x00}, // SPIWrite	064,00
		{0x80, 0x65, 0x98}, // SPIWrite	065,98	// Disable clock to Tx Filter

		//************************************************************
		// Program Rx FIR: C:\Program Files (x86)\Analog Devices\AD9361R2
		// Evaluation Software 2.1.3\DigitalFilters\0905-40-64(2).ftr
		//************************************************************
		{0x80, 0xF5, 0x9A}, // SPIWrite	0F5,9A	// Enable clock to Rx FIR Filter
		{0x80, 0xF6, 0x00}, // SPIWrite	0F6,00	// Write Filter Gain setting
		{0x80, 0xF0, 0x00}, // SPIWrite	0F0,00	// Write FIR coefficient address
		{0x80, 0xF1, 0x05}, // SPIWrite	0F1,05	// Write FIR coefficient data[7:0]
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00	// Write FIR coefficient data[15:8]
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E	// Set Write EN to push data into FIR filter register map
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00	// Dummy Write to Read only register to delay ~1us
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00	// Dummy Write to Read only register to delay ~1us
		{0x80, 0xF0, 0x01}, // SPIWrite	0F0,01
		{0x80, 0xF1, 0xBF}, // SPIWrite	0F1,BF
		{0x80, 0xF2, 0x06}, // SPIWrite	0F2,06
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x02}, // SPIWrite	0F0,02
		{0x80, 0xF1, 0xFF}, // SPIWrite	0F1,FF
		{0x80, 0xF2, 0xFE}, // SPIWrite	0F2,FE
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x03}, // SPIWrite	0F0,03
		{0x80, 0xF1, 0xF4}, // SPIWrite	0F1,F4
		{0x80, 0xF2, 0xFF}, // SPIWrite	0F2,FF
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x04}, // SPIWrite	0F0,04
		{0x80, 0xF1, 0xED}, // SPIWrite	0F1,ED
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x05}, // SPIWrite	0F0,05
		{0x80, 0xF1, 0x57}, // SPIWrite	0F1,57
		{0x80, 0xF2, 0xFE}, // SPIWrite	0F2,FE
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x06}, // SPIWrite	0F0,06
		{0x80, 0xF1, 0x3C}, // SPIWrite	0F1,3C
		{0x80, 0xF2, 0x02}, // SPIWrite	0F2,02
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x07}, // SPIWrite	0F0,07
		{0x80, 0xF1, 0x5B}, // SPIWrite	0F1,5B
		{0x80, 0xF2, 0xFD}, // SPIWrite	0F2,FD
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x08}, // SPIWrite	0F0,08
		{0x80, 0xF1, 0xE6}, // SPIWrite	0F1,E6
		{0x80, 0xF2, 0x02}, // SPIWrite	0F2,02
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x09}, // SPIWrite	0F0,09
		{0x80, 0xF1, 0x06}, // SPIWrite	0F1,06
		{0x80, 0xF2, 0xFD}, // SPIWrite	0F2,FD
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x0A}, // SPIWrite	0F0,0A
		{0x80, 0xF1, 0xDB}, // SPIWrite	0F1,DB
		{0x80, 0xF2, 0x02}, // SPIWrite	0F2,02
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x0B}, // SPIWrite	0F0,0B
		{0x80, 0xF1, 0x7D}, // SPIWrite	0F1,7D
		{0x80, 0xF2, 0xFD}, // SPIWrite	0F2,FD
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x0C}, // SPIWrite	0F0,0C
		{0x80, 0xF1, 0xF1}, // SPIWrite	0F1,F1
		{0x80, 0xF2, 0x01}, // SPIWrite	0F2,01
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x0D}, // SPIWrite	0F0,0D
		{0x80, 0xF1, 0xDC}, // SPIWrite	0F1,DC
		{0x80, 0xF2, 0xFE}, // SPIWrite	0F2,FE
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x0E}, // SPIWrite	0F0,0E
		{0x80, 0xF1, 0x1E}, // SPIWrite	0F1,1E
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x0F}, // SPIWrite	0F0,0F
		{0x80, 0xF1, 0x10}, // SPIWrite	0F1,10
		{0x80, 0xF2, 0x01}, // SPIWrite	0F2,01
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x10}, // SPIWrite	0F0,10
		{0x80, 0xF1, 0xA4}, // SPIWrite	0F1,A4
		{0x80, 0xF2, 0xFD}, // SPIWrite	0F2,FD
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x11}, // SPIWrite	0F0,11
		{0x80, 0xF1, 0xB1}, // SPIWrite	0F1,B1
		{0x80, 0xF2, 0x03}, // SPIWrite	0F2,03
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x12}, // SPIWrite	0F0,12
		{0x80, 0xF1, 0x0A}, // SPIWrite	0F1,0A
		{0x80, 0xF2, 0xFB}, // SPIWrite	0F2,FB
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x13}, // SPIWrite	0F0,13
		{0x80, 0xF1, 0x10}, // SPIWrite	0F1,10
		{0x80, 0xF2, 0x06}, // SPIWrite	0F2,06
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x14}, // SPIWrite	0F0,14
		{0x80, 0xF1, 0x1F}, // SPIWrite	0F1,1F
		{0x80, 0xF2, 0xF9}, // SPIWrite	0F2,F9
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x15}, // SPIWrite	0F0,15
		{0x80, 0xF1, 0x49}, // SPIWrite	0F1,49
		{0x80, 0xF2, 0x07}, // SPIWrite	0F2,07
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x16}, // SPIWrite	0F0,16
		{0x80, 0xF1, 0xD8}, // SPIWrite	0F1,D8
		{0x80, 0xF2, 0xF8}, // SPIWrite	0F2,F8
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x17}, // SPIWrite	0F0,17
		{0x80, 0xF1, 0x5F}, // SPIWrite	0F1,5F
		{0x80, 0xF2, 0x06}, // SPIWrite	0F2,06
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x18}, // SPIWrite	0F0,18
		{0x80, 0xF1, 0x33}, // SPIWrite	0F1,33
		{0x80, 0xF2, 0xFB}, // SPIWrite	0F2,FB
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x19}, // SPIWrite	0F0,19
		{0x80, 0xF1, 0x52}, // SPIWrite	0F1,52
		{0x80, 0xF2, 0x02}, // SPIWrite	0F2,02
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x1A}, // SPIWrite	0F0,1A
		{0x80, 0xF1, 0x33}, // SPIWrite	0F1,33
		{0x80, 0xF2, 0x01}, // SPIWrite	0F2,01
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x1B}, // SPIWrite	0F0,1B
		{0x80, 0xF1, 0x1B}, // SPIWrite	0F1,1B
		{0x80, 0xF2, 0xFA}, // SPIWrite	0F2,FA
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x1C}, // SPIWrite	0F0,1C
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x0C}, // SPIWrite	0F2,0C
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x1D}, // SPIWrite	0F0,1D
		{0x80, 0xF1, 0x8C}, // SPIWrite	0F1,8C
		{0x80, 0xF2, 0xEC}, // SPIWrite	0F2,EC
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x1E}, // SPIWrite	0F0,1E
		{0x80, 0xF1, 0x60}, // SPIWrite	0F1,60
		{0x80, 0xF2, 0x1C}, // SPIWrite	0F2,1C
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x1F}, // SPIWrite	0F0,1F
		{0x80, 0xF1, 0x9C}, // SPIWrite	0F1,9C
		{0x80, 0xF2, 0xD6}, // SPIWrite	0F2,D6
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x20}, // SPIWrite	0F0,20
		{0x80, 0xF1, 0x7A}, // SPIWrite	0F1,7A
		{0x80, 0xF2, 0x59}, // SPIWrite	0F2,59
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x21}, // SPIWrite	0F0,21
		{0x80, 0xF1, 0x7A}, // SPIWrite	0F1,7A
		{0x80, 0xF2, 0x59}, // SPIWrite	0F2,59
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x22}, // SPIWrite	0F0,22
		{0x80, 0xF1, 0x9C}, // SPIWrite	0F1,9C
		{0x80, 0xF2, 0xD6}, // SPIWrite	0F2,D6
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x23}, // SPIWrite	0F0,23
		{0x80, 0xF1, 0x60}, // SPIWrite	0F1,60
		{0x80, 0xF2, 0x1C}, // SPIWrite	0F2,1C
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x24}, // SPIWrite	0F0,24
		{0x80, 0xF1, 0x8C}, // SPIWrite	0F1,8C
		{0x80, 0xF2, 0xEC}, // SPIWrite	0F2,EC
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x25}, // SPIWrite	0F0,25
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x0C}, // SPIWrite	0F2,0C
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x26}, // SPIWrite	0F0,26
		{0x80, 0xF1, 0x1B}, // SPIWrite	0F1,1B
		{0x80, 0xF2, 0xFA}, // SPIWrite	0F2,FA
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x27}, // SPIWrite	0F0,27
		{0x80, 0xF1, 0x33}, // SPIWrite	0F1,33
		{0x80, 0xF2, 0x01}, // SPIWrite	0F2,01
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x28}, // SPIWrite	0F0,28
		{0x80, 0xF1, 0x52}, // SPIWrite	0F1,52
		{0x80, 0xF2, 0x02}, // SPIWrite	0F2,02
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x29}, // SPIWrite	0F0,29
		{0x80, 0xF1, 0x33}, // SPIWrite	0F1,33
		{0x80, 0xF2, 0xFB}, // SPIWrite	0F2,FB
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x2A}, // SPIWrite	0F0,2A
		{0x80, 0xF1, 0x5F}, // SPIWrite	0F1,5F
		{0x80, 0xF2, 0x06}, // SPIWrite	0F2,06
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x2B}, // SPIWrite	0F0,2B
		{0x80, 0xF1, 0xD8}, // SPIWrite	0F1,D8
		{0x80, 0xF2, 0xF8}, // SPIWrite	0F2,F8
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x2C}, // SPIWrite	0F0,2C
		{0x80, 0xF1, 0x49}, // SPIWrite	0F1,49
		{0x80, 0xF2, 0x07}, // SPIWrite	0F2,07
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x2D}, // SPIWrite	0F0,2D
		{0x80, 0xF1, 0x1F}, // SPIWrite	0F1,1F
		{0x80, 0xF2, 0xF9}, // SPIWrite	0F2,F9
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x2E}, // SPIWrite	0F0,2E
		{0x80, 0xF1, 0x10}, // SPIWrite	0F1,10
		{0x80, 0xF2, 0x06}, // SPIWrite	0F2,06
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x2F}, // SPIWrite	0F0,2F
		{0x80, 0xF1, 0x0A}, // SPIWrite	0F1,0A
		{0x80, 0xF2, 0xFB}, // SPIWrite	0F2,FB
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x30}, // SPIWrite	0F0,30
		{0x80, 0xF1, 0xB1}, // SPIWrite	0F1,B1
		{0x80, 0xF2, 0x03}, // SPIWrite	0F2,03
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x31}, // SPIWrite	0F0,31
		{0x80, 0xF1, 0xA4}, // SPIWrite	0F1,A4
		{0x80, 0xF2, 0xFD}, // SPIWrite	0F2,FD
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x32}, // SPIWrite	0F0,32
		{0x80, 0xF1, 0x10}, // SPIWrite	0F1,10
		{0x80, 0xF2, 0x01}, // SPIWrite	0F2,01
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x33}, // SPIWrite	0F0,33
		{0x80, 0xF1, 0x1E}, // SPIWrite	0F1,1E
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x34}, // SPIWrite	0F0,34
		{0x80, 0xF1, 0xDC}, // SPIWrite	0F1,DC
		{0x80, 0xF2, 0xFE}, // SPIWrite	0F2,FE
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x35}, // SPIWrite	0F0,35
		{0x80, 0xF1, 0xF1}, // SPIWrite	0F1,F1
		{0x80, 0xF2, 0x01}, // SPIWrite	0F2,01
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x36}, // SPIWrite	0F0,36
		{0x80, 0xF1, 0x7D}, // SPIWrite	0F1,7D
		{0x80, 0xF2, 0xFD}, // SPIWrite	0F2,FD
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x37}, // SPIWrite	0F0,37
		{0x80, 0xF1, 0xDB}, // SPIWrite	0F1,DB
		{0x80, 0xF2, 0x02}, // SPIWrite	0F2,02
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x38}, // SPIWrite	0F0,38
		{0x80, 0xF1, 0x06}, // SPIWrite	0F1,06
		{0x80, 0xF2, 0xFD}, // SPIWrite	0F2,FD
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x39}, // SPIWrite	0F0,39
		{0x80, 0xF1, 0xE6}, // SPIWrite	0F1,E6
		{0x80, 0xF2, 0x02}, // SPIWrite	0F2,02
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x3A}, // SPIWrite	0F0,3A
		{0x80, 0xF1, 0x5B}, // SPIWrite	0F1,5B
		{0x80, 0xF2, 0xFD}, // SPIWrite	0F2,FD
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x3B}, // SPIWrite	0F0,3B
		{0x80, 0xF1, 0x3C}, // SPIWrite	0F1,3C
		{0x80, 0xF2, 0x02}, // SPIWrite	0F2,02
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x3C}, // SPIWrite	0F0,3C
		{0x80, 0xF1, 0x57}, // SPIWrite	0F1,57
		{0x80, 0xF2, 0xFE}, // SPIWrite	0F2,FE
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x3D}, // SPIWrite	0F0,3D
		{0x80, 0xF1, 0xED}, // SPIWrite	0F1,ED
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x3E}, // SPIWrite	0F0,3E
		{0x80, 0xF1, 0xF4}, // SPIWrite	0F1,F4
		{0x80, 0xF2, 0xFF}, // SPIWrite	0F2,FF
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x3F}, // SPIWrite	0F0,3F
		{0x80, 0xF1, 0xFF}, // SPIWrite	0F1,FF
		{0x80, 0xF2, 0xFE}, // SPIWrite	0F2,FE
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x40}, // SPIWrite	0F0,40
		{0x80, 0xF1, 0xBF}, // SPIWrite	0F1,BF
		{0x80, 0xF2, 0x06}, // SPIWrite	0F2,06
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x41}, // SPIWrite	0F0,41
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x42}, // SPIWrite	0F0,42
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x43}, // SPIWrite	0F0,43
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x44}, // SPIWrite	0F0,44
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x45}, // SPIWrite	0F0,45
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x46}, // SPIWrite	0F0,46
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x47}, // SPIWrite	0F0,47
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x48}, // SPIWrite	0F0,48
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x49}, // SPIWrite	0F0,49
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x4A}, // SPIWrite	0F0,4A
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x4B}, // SPIWrite	0F0,4B
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x4C}, // SPIWrite	0F0,4C
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
	};
	row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
	for (i = 0; i < row; i++)
	{
		SetAd(Ad9361Config[i]);
	}
}

void Ad9361Config_03(void)
{
	u16 row, i;
	u8 Ad9361Config[][3] = {
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x4D}, // SPIWrite	0F0,4D
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x4E}, // SPIWrite	0F0,4E
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF0, 0x4F}, // SPIWrite	0F0,4F
		{0x80, 0xF1, 0x00}, // SPIWrite	0F1,00
		{0x80, 0xF2, 0x00}, // SPIWrite	0F2,00
		{0x80, 0xF5, 0x9E}, // SPIWrite	0F5,9E
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF4, 0x00}, // SPIWrite	0F4,00
		{0x80, 0xF5, 0x98}, // SPIWrite	0F5,98	// Disable clock to Rx Filter

		//************************************************************
		// Setup the Parallel Port (Digital Data Interface)
		//************************************************************
		{0x80, 0x10, 0xC8}, // SPIWrite	010,C8	// I/O Config.  Tx Swap IQ; Rx Swap IQ; Tx CH Swap, Rx CH Swap; Rx Frame Mode; 2R2T bit; Invert data bus; Invert DATA_CLK
		{0x80, 0x11, 0x00}, // SPIWrite	011,00	// I/O Config.  Alt Word Order; -Rx1; -Rx2; -Tx1; -Tx2; Invert Rx Frame; Delay Rx Data
		{0x80, 0x12, 0x42}, // SPIWrite	012,42	// I/O Config.  Rx=2*Tx; Swap Ports; SDR; LVDS; Half Duplex; Single Port; Full Port; Swap Bits
		{0x80, 0x06, 0x80}, // SPIWrite	006,80	// PPORT Rx Delay (adjusts Tco Dataclk->Data)
		{0x80, 0x07, 0x84}, // SPIWrite	007,84	// PPORT TX Delay (adjusts setup/hold FBCLK->Data)

		//************************************************************
		// Setup AuxDAC
		//************************************************************
		{0x80, 0x18, 0x00}, // SPIWrite	018,00	// AuxDAC1 Word[9:2]
		{0x80, 0x19, 0x00}, // SPIWrite	019,00	// AuxDAC2 Word[9:2]
		{0x80, 0x1A, 0x00}, // SPIWrite	01A,00	// AuxDAC1 Config and Word[1:0]
		{0x80, 0x1B, 0x00}, // SPIWrite	01B,00	// AuxDAC2 Config and Word[1:0]
		{0x80, 0x23, 0xFF}, // SPIWrite	023,FF	// AuxDAC Manaul/Auto Control
		{0x80, 0x26, 0x00}, // SPIWrite	026,00	// AuxDAC Manual Select Bit/GPO Manual Select
		{0x80, 0x30, 0x00}, // SPIWrite	030,00	// AuxDAC1 Rx Delay
		{0x80, 0x31, 0x00}, // SPIWrite	031,00	// AuxDAC1 Tx Delay
		{0x80, 0x32, 0x00}, // SPIWrite	032,00	// AuxDAC2 Rx Delay
		{0x80, 0x33, 0x00}, // SPIWrite	033,00	// AuxDAC2 Tx Delay

		//************************************************************
		// Setup AuxADC
		//************************************************************
		{0x80, 0x0B, 0x00}, // SPIWrite	00B,00	// Temp Sensor Setup (Offset)
		{0x80, 0x0C, 0x00}, // SPIWrite	00C,00	// Temp Sensor Setup (Temp Window)
		{0x80, 0x0D, 0x03}, // SPIWrite	00D,03	// Temp Sensor Setup (Periodic Measure)
		{0x80, 0x0F, 0x04}, // SPIWrite	00F,04	// Temp Sensor Setup (Decimation)
		{0x80, 0x1C, 0x10}, // SPIWrite	01C,10	// AuxADC Setup (Clock Div)
		{0x80, 0x1D, 0x01}, // SPIWrite	01D,01	// AuxADC Setup (Decimation/Enable)

		//************************************************************
		// Setup Control Outs
		//************************************************************
		{0x80, 0x35, 0x00}, // SPIWrite	035,00	// Ctrl Out index
		{0x80, 0x36, 0xFF}, // SPIWrite	036,FF	// Ctrl Out [7:0] output enable

		//************************************************************
		// Setup GPO
		//************************************************************
		{0x80, 0x3A, 0x27}, // SPIWrite	03A,27	// Set number of REFCLK cycles for 1us delay timer
		{0x80, 0x20, 0x00}, // SPIWrite	020,00	// GPO Auto Enable Setup in RX and TX
		{0x80, 0x27, 0x03}, // SPIWrite	027,03	// GPO Manual and GPO auto value in ALERT
		{0x80, 0x28, 0x00}, // SPIWrite	028,00	// GPO_0 RX Delay
		{0x80, 0x29, 0x00}, // SPIWrite	029,00	// GPO_1 RX Delay
		{0x80, 0x2A, 0x00}, // SPIWrite	02A,00	// GPO_2 RX Delay
		{0x80, 0x2B, 0x00}, // SPIWrite	02B,00	// GPO_3 RX Delay
		{0x80, 0x2C, 0x00}, // SPIWrite	02C,00	// GPO_0 TX Delay
		{0x80, 0x2D, 0x00}, // SPIWrite	02D,00	// GPO_1 TX Delay
		{0x80, 0x2E, 0x00}, // SPIWrite	02E,00	// GPO_2 TX Delay
		{0x80, 0x2F, 0x00}, // SPIWrite	02F,00	// GPO_3 TX Delay

		//************************************************************
		// Setup RF PLL non-frequency-dependent registers
		//************************************************************
		{0x82, 0x61, 0x00}, // SPIWrite	261,00	// Set Rx LO Power mode
		{0x82, 0xA1, 0x00}, // SPIWrite	2A1,00	// Set Tx LO Power mode
		{0x82, 0x48, 0x0B}, // SPIWrite	248,0B	// Enable Rx VCO LDO
		{0x82, 0x88, 0x0B}, // SPIWrite	288,0B	// Enable Tx VCO LDO
		{0x82, 0x46, 0x02}, // SPIWrite	246,02	// Set VCO Power down TCF bits
		{0x82, 0x86, 0x02}, // SPIWrite	286,02	// Set VCO Power down TCF bits
		{0x82, 0x49, 0x8E}, // SPIWrite	249,8E	// Set VCO cal length
		{0x82, 0x89, 0x8E}, // SPIWrite	289,8E	// Set VCO cal length
		{0x82, 0x3B, 0x80}, // SPIWrite	23B,80	// Enable Rx VCO cal
		{0x82, 0x7B, 0x80}, // SPIWrite	27B,80	// Enable Tx VCO cal
		{0x82, 0x43, 0x0D}, // SPIWrite	243,0D	// Set Rx prescaler bias
		{0x82, 0x83, 0x0D}, // SPIWrite	283,0D	// Set Tx prescaler bias
		{0x82, 0x3D, 0x00}, // SPIWrite	23D,00	// Clear Half VCO cal clock setting
		{0x82, 0x7D, 0x00}, // SPIWrite	27D,00	// Clear Half VCO cal clock setting

		{0x80, 0x15, 0x0C}, // SPIWrite	015,0C	// Set Dual Synth mode bit
		{0x80, 0x14, 0x15}, // SPIWrite	014,15	// Set Force ALERT State bit
		{0x80, 0x13, 0x01}, // SPIWrite	013,01	// Set ENSM FDD mode
		{0xF0, 0x00, 0x01}, // WAIT	1	// waits 1 ms

		{0x82, 0x3D, 0x04}, // SPIWrite	23D,04	// Start RX CP cal
		{0x02, 0x44, 0x71}, // WAIT_CALDONE	RXCP,100	// Wait for CP cal to complete, Max RXCP Cal time: 460.800 (us)(Done when 0x244[7]==1)

		{0x82, 0x7D, 0x04}, // SPIWrite	27D,04	// Start TX CP cal
		{0x02, 0x84, 0x71}, // WAIT_CALDONE	TXCP,100	// Wait for CP cal to complete, Max TXCP Cal time: 460.800 (us)(Done when 0x284[7]==1)

		{0x82, 0x3D, 0x00}, // SPIWrite	23D,00	// Disable RX CP Calibration since the CP Cal start bit is not self-clearing.  Only important if the script is run again without restting the DUT
		{0x82, 0x7D, 0x00}, // SPIWrite	27D,00	// Disable TX CP Calibration since the CP Cal start bit is not self-clearing.  Only important if the script is run again without restting the DUT
		//************************************************************
		// FDD RX,TX Synth Frequency: 1200.000000,1400.000000 MHz
		//************************************************************
		//************************************************************
		// Setup Rx Frequency-Dependent Syntheisizer Registers
		//************************************************************
		{0x82, 0x3A, 0x4A}, // SPIWrite	23A,4A	// Set VCO Output level[3:0]
		{0x82, 0x39, 0xC0}, // SPIWrite	239,C0	// Set Init ALC Value[3:0] and VCO Varactor[3:0]
		{0x82, 0x42, 0x0D}, // SPIWrite	242,0D	// Set VCO Bias Tcf[1:0] and VCO Bias Ref[2:0]
		{0x82, 0x38, 0x68}, // SPIWrite	238,68	// Set VCO Cal Offset[3:0]
		{0x82, 0x45, 0x00}, // SPIWrite	245,00	// Set VCO Cal Ref Tcf[2:0]
		{0x82, 0x51, 0x09}, // SPIWrite	251,09	// Set VCO Varactor Reference[3:0]
		{0x82, 0x50, 0x70}, // SPIWrite	250,70	// Set VCO Varactor Ref Tcf[2:0] and VCO Varactor Offset[3:0]
		{0x82, 0x3B, 0x91}, // SPIWrite	23B,91	// Set Synth Loop Filter charge pump current (Icp)
		{0x82, 0x3E, 0xD4}, // SPIWrite	23E,D4	// Set Synth Loop Filter C2 and C1
		{0x82, 0x3F, 0xDF}, // SPIWrite	23F,DF	// Set Synth Loop Filter  R1 and C3
		{0x82, 0x40, 0x09}, // SPIWrite	240,09	// Set Synth Loop Filter R3

		//************************************************************
		// Setup Tx Frequency-Dependent Syntheisizer Registers
		//************************************************************
		{0x82, 0x7A, 0x4A}, // SPIWrite	27A,4A	// Set VCO Output level[3:0]
		{0x82, 0x79, 0xC0}, // SPIWrite	279,C0	// Set Init ALC Value[3:0] and VCO Varactor[3:0]
		{0x82, 0x82, 0x04}, // SPIWrite	282,04	// Set VCO Bias Tcf[1:0] and VCO Bias Ref[2:0]
		{0x82, 0x78, 0x70}, // SPIWrite	278,70	// Set VCO Cal Offset[3:0]
		{0x82, 0x85, 0x00}, // SPIWrite	285,00	// Set VCO Cal Ref Tcf[2:0]
		{0x82, 0x91, 0x08}, // SPIWrite	291,08	// Set VCO Varactor Reference[3:0]
		{0x82, 0x90, 0x70}, // SPIWrite	290,70	// Set VCO Varactor Ref Tcf[2:0] and VCO Varactor Offset[3:0]
		{0x82, 0x7B, 0x8C}, // SPIWrite	27B,8C	// Set Synth Loop Filter charge pump current (Icp)
		{0x82, 0x7E, 0xD4}, // SPIWrite	27E,D4	// Set Synth Loop Filter C2 and C1
		{0x82, 0x7F, 0xDF}, // SPIWrite	27F,DF	// Set Synth Loop Filter  R1 and C3
		{0x82, 0x80, 0x09}, // SPIWrite	280,09	// Set Synth Loop Filter R3

		//************************************************************
		// Write Rx and Tx Frequency Words
		//************************************************************
		{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
		{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
		{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
		{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
		{0x82, 0x31, 0x79}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
		{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
		{0x82, 0x73, 0x00}, // SPIWrite	273,00	// Write Tx Synth Fractional Freq Word[7:0]
		{0x82, 0x74, 0x00}, // SPIWrite	274,00	// Write Tx Synth Fractional Freq Word[15:8]
		{0x82, 0x75, 0x00}, // SPIWrite	275,00	// Write Tx Synth Fractional Freq Word[22:16]
		{0x82, 0x72, 0x00}, // SPIWrite	272,00	// Write Tx Synth Integer Freq Word[10:8]
		{0x82, 0x71, 0x8C}, // SPIWrite	271,8C	// Write Tx Synth Integer Freq Word[7:0] (starts VCO cal)
		{0x80, 0x05, 0x21}, // SPIWrite	005,22	// Set LO divider setting
		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
		{0x02, 0x87, 0x11}, // SPIRead	287	// Check TX RF PLL lock status (0x287[1]==1 is locked)

		//************************************************************
		// Program Mixer GM Sub-table
		//************************************************************
		{0x81, 0x3F, 0x02}, // SPIWrite	13F,02	// Start Clock
		{0x81, 0x38, 0x0F}, // SPIWrite	138,0F	// Addr Table Index
		{0x81, 0x39, 0x78}, // SPIWrite	139,78	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x00}, // SPIWrite	13B,00	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x0E}, // SPIWrite	138,0E	// Addr Table Index
		{0x81, 0x39, 0x74}, // SPIWrite	139,74	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x0D}, // SPIWrite	13B,0D	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x0D}, // SPIWrite	138,0D	// Addr Table Index
		{0x81, 0x39, 0x70}, // SPIWrite	139,70	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x15}, // SPIWrite	13B,15	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x0C}, // SPIWrite	138,0C	// Addr Table Index
		{0x81, 0x39, 0x6C}, // SPIWrite	139,6C	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x1B}, // SPIWrite	13B,1B	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x0B}, // SPIWrite	138,0B	// Addr Table Index
		{0x81, 0x39, 0x68}, // SPIWrite	139,68	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x21}, // SPIWrite	13B,21	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x0A}, // SPIWrite	138,0A	// Addr Table Index
		{0x81, 0x39, 0x64}, // SPIWrite	139,64	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x25}, // SPIWrite	13B,25	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x09}, // SPIWrite	138,09	// Addr Table Index
		{0x81, 0x39, 0x60}, // SPIWrite	139,60	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x29}, // SPIWrite	13B,29	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x08}, // SPIWrite	138,08	// Addr Table Index
		{0x81, 0x39, 0x5C}, // SPIWrite	139,5C	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x2C}, // SPIWrite	13B,2C	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x07}, // SPIWrite	138,07	// Addr Table Index
		{0x81, 0x39, 0x58}, // SPIWrite	139,58	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x2F}, // SPIWrite	13B,2F	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x06}, // SPIWrite	138,06	// Addr Table Index
		{0x81, 0x39, 0x54}, // SPIWrite	139,54	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x31}, // SPIWrite	13B,31	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x05}, // SPIWrite	138,05	// Addr Table Index
		{0x81, 0x39, 0x50}, // SPIWrite	139,50	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x33}, // SPIWrite	13B,33	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x04}, // SPIWrite	138,04	// Addr Table Index
		{0x81, 0x39, 0x4C}, // SPIWrite	139,4C	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x34}, // SPIWrite	13B,34	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x03}, // SPIWrite	138,03	// Addr Table Index
		{0x81, 0x39, 0x48}, // SPIWrite	139,48	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x35}, // SPIWrite	13B,35	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x02}, // SPIWrite	138,02	// Addr Table Index
		{0x81, 0x39, 0x30}, // SPIWrite	139,30	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x3A}, // SPIWrite	13B,3A	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x01}, // SPIWrite	138,01	// Addr Table Index
		{0x81, 0x39, 0x18}, // SPIWrite	139,18	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x3D}, // SPIWrite	13B,3D	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x38, 0x00}, // SPIWrite	138,00	// Addr Table Index
		{0x81, 0x39, 0x00}, // SPIWrite	139,00	// Gain
		{0x81, 0x3A, 0x00}, // SPIWrite	13A,00	// Bias
		{0x81, 0x3B, 0x3E}, // SPIWrite	13B,3E	// GM
		{0x81, 0x3F, 0x06}, // SPIWrite	13F,06	// Write Words
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x3F, 0x02}, // SPIWrite	13F,02	// Clear Write Bit
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81, 0x3C, 0x00}, // SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81, 0x3F, 0x00}, // SPIWrite	13F,00	// Stop Clock

		//************************************************************
		// Program Rx Gain Tables with GainTable800MHz.csv
		//************************************************************

		{0x81, 0x37, 0x1A}, // SPIWrite	137,1A	// Start Gain Table Clock
		{0x81, 0x30, 0x00}, // SPIWrite	130,00	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x01}, // SPIWrite	130,01	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x02}, // SPIWrite	130,02	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x03}, // SPIWrite	130,03	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x01}, // SPIWrite	132,01	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x04}, // SPIWrite	130,04	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x02}, // SPIWrite	132,02	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x05}, // SPIWrite	130,05	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x03}, // SPIWrite	132,03	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x06}, // SPIWrite	130,06	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x04}, // SPIWrite	132,04	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x07}, // SPIWrite	130,07	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x05}, // SPIWrite	132,05	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x08}, // SPIWrite	130,08	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x03}, // SPIWrite	132,03	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x09}, // SPIWrite	130,09	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x04}, // SPIWrite	132,04	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x0A}, // SPIWrite	130,0A	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x05}, // SPIWrite	132,05	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x0B}, // SPIWrite	130,0B	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x06}, // SPIWrite	132,06	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x0C}, // SPIWrite	130,0C	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x07}, // SPIWrite	132,07	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x0D}, // SPIWrite	130,0D	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x08}, // SPIWrite	132,08	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x0E}, // SPIWrite	130,0E	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x09}, // SPIWrite	132,09	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x0F}, // SPIWrite	130,0F	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0A}, // SPIWrite	132,0A	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x10}, // SPIWrite	130,10	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0B}, // SPIWrite	132,0B	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x11}, // SPIWrite	130,11	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0C}, // SPIWrite	132,0C	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x12}, // SPIWrite	130,12	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0D}, // SPIWrite	132,0D	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x13}, // SPIWrite	130,13	// Gain Table Index
		{0x81, 0x31, 0x01}, // SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0E}, // SPIWrite	132,0E	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x14}, // SPIWrite	130,14	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x09}, // SPIWrite	132,09	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x15}, // SPIWrite	130,15	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0A}, // SPIWrite	132,0A	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x16}, // SPIWrite	130,16	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0B}, // SPIWrite	132,0B	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x17}, // SPIWrite	130,17	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0C}, // SPIWrite	132,0C	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x18}, // SPIWrite	130,18	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0D}, // SPIWrite	132,0D	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x19}, // SPIWrite	130,19	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0E}, // SPIWrite	132,0E	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x1A}, // SPIWrite	130,1A	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x0F}, // SPIWrite	132,0F	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x1B}, // SPIWrite	130,1B	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x10}, // SPIWrite	132,10	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x1C}, // SPIWrite	130,1C	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2B}, // SPIWrite	132,2B	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x1D}, // SPIWrite	130,1D	// Gain Table Index
		{0x81, 0x31, 0x02}, // SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2C}, // SPIWrite	132,2C	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x1E}, // SPIWrite	130,1E	// Gain Table Index
		{0x81, 0x31, 0x04}, // SPIWrite	131,04	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x28}, // SPIWrite	132,28	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
	};
	row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
	for (i = 0; i < row; i++)
	{
		SetAd(Ad9361Config[i]);
	}
}

void Ad9361Config_04(void)
{
	u16 row, i;
	u8 Ad9361Config[][3] = {
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x1F}, // SPIWrite	130,1F	// Gain Table Index
		{0x81, 0x31, 0x04}, // SPIWrite	131,04	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x29}, // SPIWrite	132,29	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x20}, // SPIWrite	130,20	// Gain Table Index
		{0x81, 0x31, 0x04}, // SPIWrite	131,04	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2A}, // SPIWrite	132,2A	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x21}, // SPIWrite	130,21	// Gain Table Index
		{0x81, 0x31, 0x04}, // SPIWrite	131,04	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2B}, // SPIWrite	132,2B	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x22}, // SPIWrite	130,22	// Gain Table Index
		{0x81, 0x31, 0x24}, // SPIWrite	131,24	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x20}, // SPIWrite	132,20	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x23}, // SPIWrite	130,23	// Gain Table Index
		{0x81, 0x31, 0x24}, // SPIWrite	131,24	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x21}, // SPIWrite	132,21	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x24}, // SPIWrite	130,24	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x20}, // SPIWrite	132,20	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x25}, // SPIWrite	130,25	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x21}, // SPIWrite	132,21	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x26}, // SPIWrite	130,26	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x22}, // SPIWrite	132,22	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x27}, // SPIWrite	130,27	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x23}, // SPIWrite	132,23	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x28}, // SPIWrite	130,28	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x24}, // SPIWrite	132,24	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x29}, // SPIWrite	130,29	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x25}, // SPIWrite	132,25	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x2A}, // SPIWrite	130,2A	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x26}, // SPIWrite	132,26	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x2B}, // SPIWrite	130,2B	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x27}, // SPIWrite	132,27	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x2C}, // SPIWrite	130,2C	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x28}, // SPIWrite	132,28	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x2D}, // SPIWrite	130,2D	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x29}, // SPIWrite	132,29	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x2E}, // SPIWrite	130,2E	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2A}, // SPIWrite	132,2A	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x2F}, // SPIWrite	130,2F	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2B}, // SPIWrite	132,2B	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x30}, // SPIWrite	130,30	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2C}, // SPIWrite	132,2C	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x31}, // SPIWrite	130,31	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2D}, // SPIWrite	132,2D	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x32}, // SPIWrite	130,32	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2E}, // SPIWrite	132,2E	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x33}, // SPIWrite	130,33	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2F}, // SPIWrite	132,2F	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x34}, // SPIWrite	130,34	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x30}, // SPIWrite	132,30	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x35}, // SPIWrite	130,35	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x31}, // SPIWrite	132,31	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x36}, // SPIWrite	130,36	// Gain Table Index
		{0x81, 0x31, 0x44}, // SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x32}, // SPIWrite	132,32	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x37}, // SPIWrite	130,37	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2E}, // SPIWrite	132,2E	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x38}, // SPIWrite	130,38	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x2F}, // SPIWrite	132,2F	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x39}, // SPIWrite	130,39	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x30}, // SPIWrite	132,30	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x3A}, // SPIWrite	130,3A	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x31}, // SPIWrite	132,31	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x3B}, // SPIWrite	130,3B	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x32}, // SPIWrite	132,32	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x3C}, // SPIWrite	130,3C	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x33}, // SPIWrite	132,33	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x3D}, // SPIWrite	130,3D	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x34}, // SPIWrite	132,34	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x3E}, // SPIWrite	130,3E	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x35}, // SPIWrite	132,35	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x3F}, // SPIWrite	130,3F	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x36}, // SPIWrite	132,36	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x40}, // SPIWrite	130,40	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x37}, // SPIWrite	132,37	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x41}, // SPIWrite	130,41	// Gain Table Index
		{0x81, 0x31, 0x64}, // SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x42}, // SPIWrite	130,42	// Gain Table Index
		{0x81, 0x31, 0x65}, // SPIWrite	131,65	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x43}, // SPIWrite	130,43	// Gain Table Index
		{0x81, 0x31, 0x66}, // SPIWrite	131,66	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x44}, // SPIWrite	130,44	// Gain Table Index
		{0x81, 0x31, 0x67}, // SPIWrite	131,67	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x45}, // SPIWrite	130,45	// Gain Table Index
		{0x81, 0x31, 0x68}, // SPIWrite	131,68	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x46}, // SPIWrite	130,46	// Gain Table Index
		{0x81, 0x31, 0x69}, // SPIWrite	131,69	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x47}, // SPIWrite	130,47	// Gain Table Index
		{0x81, 0x31, 0x6A}, // SPIWrite	131,6A	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x48}, // SPIWrite	130,48	// Gain Table Index
		{0x81, 0x31, 0x6B}, // SPIWrite	131,6B	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x49}, // SPIWrite	130,49	// Gain Table Index
		{0x81, 0x31, 0x6C}, // SPIWrite	131,6C	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x4A}, // SPIWrite	130,4A	// Gain Table Index
		{0x81, 0x31, 0x6D}, // SPIWrite	131,6D	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x4B}, // SPIWrite	130,4B	// Gain Table Index
		{0x81, 0x31, 0x6E}, // SPIWrite	131,6E	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x4C}, // SPIWrite	130,4C	// Gain Table Index
		{0x81, 0x31, 0x6F}, // SPIWrite	131,6F	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x38}, // SPIWrite	132,38	// TIA & LPF Word
		{0x81, 0x33, 0x20}, // SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x4D}, // SPIWrite	130,4D	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x4E}, // SPIWrite	130,4E	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x4F}, // SPIWrite	130,4F	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x50}, // SPIWrite	130,50	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x51}, // SPIWrite	130,51	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x52}, // SPIWrite	130,52	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x53}, // SPIWrite	130,53	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x54}, // SPIWrite	130,54	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x55}, // SPIWrite	130,55	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x56}, // SPIWrite	130,56	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x57}, // SPIWrite	130,57	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x58}, // SPIWrite	130,58	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x59}, // SPIWrite	130,59	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x30, 0x5A}, // SPIWrite	130,5A	// Gain Table Index
		{0x81, 0x31, 0x00}, // SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81, 0x32, 0x00}, // SPIWrite	132,00	// TIA & LPF Word
		{0x81, 0x33, 0x00}, // SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81, 0x37, 0x1E}, // SPIWrite	137,1E	// Write Words
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x37, 0x1A}, // SPIWrite	137,1A	// Clear Write Bit
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x34, 0x00}, // SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81, 0x37, 0x00}, // SPIWrite	137,00	// Stop Gain Table Clock
//		//************************************************************
//		// Setup Rx AGC Fast AttackRegisters
//		//************************************************************
//		{0x80, 0x22, 0x0A}, // SPIWrite	022,0A	// AGC Fast Attack Gain Lock Delay
//		{0x80, 0xFA, 0xE5}, // SPIWrite	0FA,E5	// Gain Control Mode Select
//		{0x80, 0xFB, 0x08}, // SPIWrite	0FB,08	// Gain Control Config
//		{0x80, 0xFC, 0x23}, // SPIWrite	0FC,23	// ADC Overrange Sample Size
//		{0x80, 0xFD, 0x4C}, // SPIWrite	0FD,4C	// Max Full/LMT Gain Table Index
//		{0x80, 0xFE, 0x44}, // SPIWrite	0FE,44	// Peak Overload Wait Time
//		{0x81, 0x00, 0x6F}, // SPIWrite	100,6F	// Dig Gain: Step Size & Max
//		{0x81, 0x01, 0x0A}, // SPIWrite	101,0A	// AGC Lock Level
//		{0x81, 0x03, 0x08}, // SPIWrite	103,08	// Large LMT or Step 3 Size
//		{0x81, 0x04, 0x2F}, // SPIWrite	104,2F	// ADC Small Overload Threshold
//		{0x81, 0x05, 0x3A}, // SPIWrite	105,3A	// ADC Large Overload Threshold
//		{0x81, 0x06, 0x22}, // SPIWrite	106,22	// Overload Step Sizes
//		{0x81, 0x07, 0x2B}, // SPIWrite	107,2B	// Small LMT Overload Threshold
//		{0x81, 0x08, 0x31}, // SPIWrite	108,31	// Large LMT Overload Threshold
//		{0x81, 0x09, 0x4C}, // SPIWrite	109,4C	// State 5 Power Measurement MSB
//		{0x81, 0x0A, 0x58}, // SPIWrite	10A,58	// State 5 Power Measurement LSBs
//		{0x81, 0x0B, 0x00}, // SPIWrite	10B,00	// Rx1 Force Digital Gain
//		{0x81, 0x0E, 0x00}, // SPIWrite	10E,00	// Rx2 Force Digital Gain
//		{0x81, 0x10, 0x00}, // SPIWrite	110,00	// AGC Fast Attack Config
//		{0x81, 0x11, 0x0A}, // SPIWrite	111,0A	// Settling Delay & AGC Config
//		{0x81, 0x12, 0x52}, // SPIWrite	112,52	// Post Lock Step & Energy Lost Thresh
//		{0x81, 0x13, 0x4C}, // SPIWrite	113,4C	// Post Lock Step & Strong Sig Thresh
//		{0x81, 0x14, 0x30}, // SPIWrite	114,30	// Low Power Threshold & ADC Ovr Ld
//		{0x81, 0x15, 0x00}, // SPIWrite	115,00	// Stronger Signal Unlock Control
//		{0x81, 0x16, 0x65}, // SPIWrite	116,65	// Final Overrange and Opt Gain Offset
//		{0x81, 0x17, 0x08}, // SPIWrite	117,08	// Gain Inc Step & Energy Detect Cnt
//		{0x81, 0x18, 0x05}, // SPIWrite	118,05	// Lock Level GAin Incr Upper Limit
//		{0x81, 0x19, 0x08}, // SPIWrite	119,08	// Gain Lock Exit Count
//		{0x81, 0x1A, 0x27}, // SPIWrite	11A,27	// Initial LMT Gain Limit
//		{0x81, 0x1B, 0x0A}, // SPIWrite	11B,0A	// Increment Time

		//************************************************************
		// Setup Rx Manual Gain Registers
		//************************************************************
		{0x80, 0xFA, 0xE0}, // SPIWrite	0FA,E0	// Gain Control Mode Select
		{0x80, 0xFB, 0x08}, // SPIWrite	0FB,08	// Table, Digital Gain, Man Gain Ctrl
		{0x80, 0xFC, 0x23}, // SPIWrite	0FC,23	// Incr Step Size, ADC Overrange Size
		{0x80, 0xFD, 0x4C}, // SPIWrite	0FD,4C	// Max Full/LMT Gain Table Index
		{0x80, 0xFE, 0x44}, // SPIWrite	0FE,44	// Decr Step Size, Peak Overload Time
		{0x81, 0x00, 0x6F}, // SPIWrite	100,6F	// Max Digital Gain
		{0x81, 0x04, 0x2F}, // SPIWrite	104,2F	// ADC Small Overload Threshold
		{0x81, 0x05, 0x3A}, // SPIWrite	105,3A	// ADC Large Overload Threshold
		{0x81, 0x07, 0x2B}, // SPIWrite	107,2B	// Small LMT Overload Threshold
		{0x81, 0x08, 0x31}, // SPIWrite	108,31	// Large LMT Overload Threshold
		{0x81, 0x09, 0x3C}, // SPIWrite	109,0A	// Rx1 Full/LMT Gain Index
		{0x81, 0x0A, 0x58}, // SPIWrite	10A,58	// Rx1 LPF Gain Index
		{0x81, 0x0B, 0x00}, // SPIWrite	10B,00	// Rx1 Digital Gain Index
		{0x81, 0x0C, 0x3C}, // SPIWrite	10C,0A	// Rx2 Full/LMT Gain Index
		{0x81, 0x0D, 0x18}, // SPIWrite	10D,18	// Rx2 LPF Gain Index
		{0x81, 0x0E, 0x00}, // SPIWrite	10E,00	// Rx2 Digital Gain Index
		{0x81, 0x14, 0x30}, // SPIWrite	114,30	// Low Power Threshold
		{0x81, 0x1A, 0x27}, // SPIWrite	11A,27	// Initial LMT Gain Limit
		{0x80, 0x81, 0x00}, // SPIWrite	081,00	// Tx Symbol Gain Control
		//************************************************************
		// RX Baseband Filter Tuning (Real BW: 10.000000 MHz) 3dB Filter
		// Corner @ 14.000000 MHz)
		//************************************************************
		{0x81, 0xFB, 0x0A}, // SPIWrite	1FB,0A	// RX Freq Corner (MHz)
		{0x81, 0xFC, 0x00}, // SPIWrite	1FC,00	// RX Freq Corner (Khz)
		{0x81, 0xF8, 0x0B}, // SPIWrite	1F8,0B	// Rx BBF Tune Divider[7:0]
		{0x81, 0xF9, 0x1E}, // SPIWrite	1F9,1E	// RX BBF Tune Divider[8]

		{0x81, 0xD5, 0x3F}, // SPIWrite	1D5,3F	// Set Rx Mix LO CM
		{0x81, 0xC0, 0x03}, // SPIWrite	1C0,03	// Set GM common mode
		{0x81, 0xE2, 0x02}, // SPIWrite	1E2,02	// Enable Rx1 Filter Tuner
		{0x81, 0xE3, 0x02}, // SPIWrite	1E3,02	// Enable Rx2 Filter Tuner
		{0x80, 0x16, 0x80}, // SPIWrite	016,80	// Start RX Filter Tune
		{0x00, 0x16, 0x70}, // WAIT_CALDONE	RXFILTER,2000	// Wait for RX filter to tune, Max Cal Time: 5.242 us (Done when 0x016[7]==0)

		{0x81, 0xE2, 0x03}, // SPIWrite	1E2,03	// Disable Rx Filter Tuner (Rx1)
		{0x81, 0xE3, 0x03}, // SPIWrite	1E3,03	// Disable Rx Filter Tuner (Rx2)
		//************************************************************
		// TX Baseband Filter Tuning (Real BW: 10.000000 MHz) 3dB Filter
		// Corner @ 16.000000 MHz)
		//************************************************************
		{0x80, 0xD6, 0x09}, // SPIWrite	0D6,09	// TX BBF Tune Divider[7:0]
		{0x80, 0xD7, 0x1E}, // SPIWrite	0D7,1E	// TX BBF Tune Divider[8]

		{0x80, 0xCA, 0x22}, // SPIWrite	0CA,22	// Enable Tx Filter Tuner
		{0x80, 0x16, 0x40}, // SPIWrite	016,40	// Start Tx Filter Tune
		{0x00, 0x16, 0x60}, // WAIT_CALDONE	TXFILTER,2000	// Wait for TX filter to tune, Max Cal Time: 2.496 us (Done when 0x016[6]==0)

		{0x80, 0xCA, 0x26}, // SPIWrite	0CA,26	// Disable Tx Filter Tuner (Both Channels)
		//************************************************************
		// RX TIA Setup:  Setup values scale based on RxBBF calibration
		// results.  See information in Calibration Guide.
		//************************************************************
		{0x01, 0xEB, 0xFF}, // SPIRead	1EB	// Read RXBBF C3(MSB)
		{0x01, 0xEC, 0xFF}, // SPIRead	1EC	// Read RXBBF C3(LSB)
		{0x01, 0xE6, 0xFF}, // SPIRead	1E6	// Read RXBBF R2346
		{0x81, 0xDB, 0x20}, // SPIWrite	1DB,20	// Set TIA selcc[2:0]
		{0x81, 0xDD, 0x00}, // SPIWrite	1DD,00	// Set RX TIA1 C MSB[6:0]
		{0x81, 0xDF, 0x00}, // SPIWrite	1DF,00	// Set RX TIA2 C MSB[6:0]
		{0x81, 0xDC, 0x63}, // SPIWrite	1DC,63	// Set RX TIA1 C LSB[5:0]
		{0x81, 0xDE, 0x63}, // SPIWrite	1DE,63	// Set RX TIA2 C LSB[5:0]

		//************************************************************
		// TX Secondary Filter Calibration Setup:  Real Bandwidth
		// 10.000000MHz, 3dB Corner @ 50.000000MHz
		//************************************************************
		{0x80, 0xD2, 0x14}, // SPIWrite	0D2,14	// TX Secondary Filter PDF Cap cal[5:0]
		{0x80, 0xD1, 0x0C}, // SPIWrite	0D1,0C	// TX Secondary Filter PDF Res cal[3:0]
		{0x80, 0xD0, 0x56}, // SPIWrite	0D0,56	// Pdampbias

		//************************************************************
	};
	row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
	for (i = 0; i < row; i++)
	{
		SetAd(Ad9361Config[i]);
	}
}

void Ad9361Config_05(void)
{
	u16 row, i;
	u8 Ad9361Config[][3] = {
		// ADC Setup:  Tune ADC Performance based on RX analog filter tune
		// corner.  Real Bandwidth: 9.169272 MHz, ADC Clock Frequency:
		// 320.000000 MHz.  The values in registers 0x200 - 0x227 need to be
		// calculated using the equations in the Calibration Guide.
		//************************************************************
		{0x01, 0xEB, 0xFF}, // SPIRead	1EB	// Read RxBBF C3 MSB after calibration
		{0x01, 0xEC, 0xFF}, // SPIRead	1EC	// Read RxBBF C3 LSB after calibration
		{0x01, 0xE6, 0xFF}, // SPIRead	1E6	// Read RxBBF R3 after calibration

		{0x82, 0x00, 0x00}, // SPIWrite	200,00
		{0x82, 0x01, 0x00}, // SPIWrite	201,00
		{0x82, 0x02, 0x00}, // SPIWrite	202,00
		{0x82, 0x03, 0x24}, // SPIWrite	203,24
		{0x82, 0x04, 0x24}, // SPIWrite	204,24
		{0x82, 0x05, 0x00}, // SPIWrite	205,00
		{0x82, 0x06, 0x00}, // SPIWrite	206,00
		{0x82, 0x07, 0x7B}, // SPIWrite	207,7B
		{0x82, 0x08, 0x40}, // SPIWrite	208,40
		{0x82, 0x09, 0x3B}, // SPIWrite	209,3B
		{0x82, 0x0A, 0x4A}, // SPIWrite	20A,4A
		{0x82, 0x0B, 0x28}, // SPIWrite	20B,28
		{0x82, 0x0C, 0x4D}, // SPIWrite	20C,4D
		{0x82, 0x0D, 0x26}, // SPIWrite	20D,26
		{0x82, 0x0E, 0x00}, // SPIWrite	20E,00
		{0x82, 0x0F, 0x7E}, // SPIWrite	20F,7E
		{0x82, 0x10, 0x7E}, // SPIWrite	210,7E
		{0x82, 0x11, 0x7E}, // SPIWrite	211,7E
		{0x82, 0x12, 0x48}, // SPIWrite	212,48
		{0x82, 0x13, 0x48}, // SPIWrite	213,48
		{0x82, 0x14, 0x48}, // SPIWrite	214,48
		{0x82, 0x15, 0x4B}, // SPIWrite	215,4B
		{0x82, 0x16, 0x4B}, // SPIWrite	216,4B
		{0x82, 0x17, 0x4B}, // SPIWrite	217,4B
		{0x82, 0x18, 0x2E}, // SPIWrite	218,2E
		{0x82, 0x19, 0x9F}, // SPIWrite	219,9F
		{0x82, 0x1A, 0x22}, // SPIWrite	21A,22
		{0x82, 0x1B, 0x16}, // SPIWrite	21B,16
		{0x82, 0x1C, 0x9F}, // SPIWrite	21C,9F
		{0x82, 0x1D, 0x22}, // SPIWrite	21D,22
		{0x82, 0x1E, 0x16}, // SPIWrite	21E,16
		{0x82, 0x1F, 0x9F}, // SPIWrite	21F,9F
		{0x82, 0x20, 0x22}, // SPIWrite	220,22
		{0x82, 0x21, 0x2C}, // SPIWrite	221,2C
		{0x82, 0x22, 0x2D}, // SPIWrite	222,2D
		{0x82, 0x23, 0x40}, // SPIWrite	223,40
		{0x82, 0x24, 0x40}, // SPIWrite	224,40
		{0x82, 0x25, 0x2C}, // SPIWrite	225,2C
		{0x82, 0x26, 0x00}, // SPIWrite	226,00
		{0x82, 0x27, 0x00}, // SPIWrite	227,00
		//************************************************************
		// Setup and Run BB DC and RF DC Offset Calibrations
		//************************************************************
		{0x81, 0x93, 0x3F}, // SPIWrite	193,3F
		{0x81, 0x90, 0x0F}, // SPIWrite	190,0F	// Set BBDC tracking shift M value, only applies when BB DC tracking enabled
		{0x81, 0x94, 0x01}, // SPIWrite	194,01	// BBDC Cal setting
		{0x80, 0x16, 0x01}, // SPIWrite	016,01	// Start BBDC offset cal
		{0x00, 0x16, 0x00}, // WAIT_CALDONE	BBDC,2000	// BBDC Max Cal Time: 10100.000 us. Cal done when 0x016[0]==0

		{0x81, 0x85, 0x20}, // SPIWrite	185,20	// Set RF DC offset Wait Count
		{0x81, 0x86, 0x32}, // SPIWrite	186,32	// Set RF DC Offset Count[7:0]
		{0x81, 0x87, 0x24}, // SPIWrite	187,24	// Settings for RF DC cal
		{0x81, 0x8B, 0x83}, // SPIWrite	18B,83	// Settings for RF DC cal
		{0x81, 0x88, 0x05}, // SPIWrite	188,05	// Settings for RF DC cal
		{0x81, 0x89, 0x30}, // SPIWrite	189,30	// Settings for RF DC cal
		{0x80, 0x16, 0x02}, // SPIWrite	016,02	// Start RFDC offset cal

		//************************************************************
		// Tx Quadrature Calibration Settings
		//************************************************************
		{0x00, 0xA3, 0xFF}, // SPIRead	0A3	// Masked Read:  Read lower 6 bits, overwrite [7:6] below
		{0x80, 0xA0, 0x3A}, // SPIWrite	0A0,3A	// Set TxQuadcal NCO frequency
		{0x80, 0xA3, 0x40}, // SPIWrite	0A3,40	// Set TxQuadcal NCO frequency (Only update bits [7:6])
		{0x80, 0xA1, 0x7B}, // SPIWrite	0A1,7B	// Tx Quad Cal Configuration, Phase and Gain Cal Enable
		{0x80, 0xA9, 0xFF}, // SPIWrite	0A9,FF	// Set Tx Quad Cal Count
		{0x80, 0xA2, 0x7F}, // SPIWrite	0A2,7F	// Set Tx Quad Cal Kexp
		{0x80, 0xA5, 0x01}, // SPIWrite	0A5,01	// Set Tx Quad Cal Magnitude Threshhold
		{0x80, 0xA6, 0x01}, // SPIWrite	0A6,01	// Set Tx Quad Cal Magnitude Threshhold
		{0x80, 0xAA, 0x22}, // SPIWrite	0AA,22	// Set Tx Quad Cal Gain Table index
		{0x80, 0xA4, 0xF0}, // SPIWrite	0A4,F0	// Set Tx Quad Cal Settle Count
		{0x80, 0xAE, 0x00}, // SPIWrite	0AE,00	// Set Tx Quad Cal LPF Gain index incase Split table mode used

		{0x81, 0x69, 0xC0}, // SPIWrite	169,C0	// Disable Rx Quadrature Calibration before Running Tx Quadrature Calibration
		{0x80, 0x16, 0x10}, // SPIWrite	016,10	// Start Tx Quad cal
		{0x00, 0x16, 0x40}, // WAIT_CALDONE	TXQUAD,2000	// Wait for cal to complete (Done when 0x016[4]==0)

		{0x81, 0x6A, 0x75}, // SPIWrite	16A,75	// Set Kexp Phase
		{0x81, 0x6B, 0x95}, // SPIWrite	16B,95	// Set Kexp Amplitude & Prevent Positive Gain Bit
		{0x81, 0x69, 0xCF}, // SPIWrite	169,CF	// Enable Rx Quadrature Calibration Tracking
		{0x81, 0x8B, 0xAD}, // SPIWrite	18B,AD	// Enable BB and RF DC Tracking Calibrations
		{0x80, 0x12, 0x42}, // SPIWrite	012,42	// Cals done, Set PPORT Config
		{0x80, 0x13, 0x01}, // SPIWrite	013,01	// Set ENSM FDD/TDD bit
		{0x80, 0x15, 0x0C}, // SPIWrite	015,0C	// Set Dual Synth Mode, FDD External Control bits properly

		//************************************************************
		// Set Tx Attenuation: Tx1: 10.00 dB,  Tx2: 10.00 dB
		//************************************************************
		{0x80, 0x73, 0x30}, // SPIWrite	073,28
		{0x80, 0x74, 0x00}, // SPIWrite	074,00
		{0x80, 0x75, 0x30}, // SPIWrite	075,28
		{0x80, 0x76, 0x00}, // SPIWrite	076,00
		//************************************************************
		// Setup RSSI and Power Measurement Duration Registers
		//************************************************************
		{0x81, 0x50, 0x0E}, // SPIWrite	150,0E	// RSSI Measurement Duration 0, 1
		{0x81, 0x51, 0x00}, // SPIWrite	151,00	// RSSI Measurement Duration 2, 3
		{0x81, 0x52, 0xFF}, // SPIWrite	152,FF	// RSSI Weighted Multiplier 0
		{0x81, 0x53, 0x00}, // SPIWrite	153,00	// RSSI Weighted Multiplier 1
		{0x81, 0x54, 0x00}, // SPIWrite	154,00	// RSSI Weighted Multiplier 2
		{0x81, 0x55, 0x00}, // SPIWrite	155,00	// RSSI Weighted Multiplier 3
		{0x81, 0x56, 0x00}, // SPIWrite	156,00	// RSSI Delay
		{0x81, 0x57, 0x00}, // SPIWrite	157,00	// RSSI Wait
		{0x81, 0x58, 0x0D}, // SPIWrite	158,0D	// RSSI Mode Select
		{0x81, 0x5C, 0x67}, // SPIWrite	15C,67	// Power Measurement Duration

	};
	row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
	for (i = 0; i < row; i++)
	{
		SetAd(Ad9361Config[i]);
	}
}

void cfgAd9361(void)
{
	//	selectSpi(2);
	Ad9361Config_01();
	Ad9361Config_02();
	Ad9361Config_03();
	Ad9361Config_04();
	Ad9361Config_05();
	//	delayMs(30);
}

int32_t InitAd9361(void)
{
	vTaskDelay(51);

//	usleep(5000);
	// zynq_uart_test();
	print("TX:reset ad936x...\r\n\r");
	reset_ad936x();
	vTaskDelay(100);

	print("TX:ad9361 cfg start...\r\n\r");
	//	cfgAd9361();
	Ad9361_fre_hop_init();
	print("TX:ad9361 cfg finish...\r\n\r");
	enable_ad936x();
	print("TX:fpga_write(0x30,0x01); \r\n");
	write_fpga_regs(0x30, 0x01);
	print("TX:fpga_write(0x31,0x09); \r\n");
	write_fpga_regs(0x31, 0x09);

	return 0;
}

int32_t InitAd9361_Rx(void)
{
	vTaskDelay(5);

//	usleep(5000);
	// zynq_uart_test();
	print("RX:reset ad936x...\r\n\r");
	reset_ad936x();
//	usleep(1000000);
	vTaskDelay(100);

	print("RX:ad9361 cfg start...\r\n\r");
	cfgAd9361();
	//	Ad9361_fre_hop_init();
	print("RX:ad9361 cfg finish...\r\n\r");
	enable_ad936x();
	print("RX:fpga_write(0x30,0x01); \r\n");
	write_fpga_regs(0x30, 0x01);
	print("RX:fpga_write(0x31,0x09); \r\n");
	write_fpga_regs(0x31, 0x09);

	return 0;
}

u8 Ad9361FreqConfig_RX_433m[][3] = {
	//		//************************************************************
	//		// Write Rx Frequency Words
	//		//************************************************************
	{0x82, 0x33, 0xC4}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x56}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x23}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_842m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x50}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x97}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x19}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x54}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_915m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x7F}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFD}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x5B}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_1200m[][3] = {
	//		//************************************************************
	//		// Write Rx Frequency Words
	//		//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x78}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_837m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x8F}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x99}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x59}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x53}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x12}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_2406m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x62}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x26}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x78}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_2420m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x79}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_2460m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7B}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5160m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x81}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5200m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x82}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5240m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x83}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5745m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8F}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5785m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x90}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5825m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x91}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5865m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x92}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_875m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x57}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x12}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_2417m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xC0}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x6C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x78}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_2447m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xC8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x2C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7A}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_2477m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xC0}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x6C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7B}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5157m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x59}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x76}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x80}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5187m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x5C}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x56}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x81}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5217m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x60}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x36}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x82}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5247m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x64}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x16}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x83}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5742m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x5E}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x46}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8F}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5772m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x62}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x26}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x90}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5802m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x66}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x06}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x91}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5832m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x5A}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x66}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x91}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5862m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x5E}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x46}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x92}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

/* Sets the TX frequency. */
void set_tx_freq(float freq)
{
}

/* Sets the RX frequency. */
uint32_t set_rx_freq(uint32_t freqItem)
{
	uint8_t i = 0;
	uint32_t freq = 0;
	uint8_t cnt = 6;
	static uint8_t GainCutoverIndex = 0;
	static uint8_t GainCnt = 0;
	enum Freq_CFG localFreqItem = (enum Freq_CFG)freqItem;

	switch (localFreqItem)
	{
		//	case Freq_RX_875m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_875m[i]);
		//		}
		//		freq = 875;
		//	}break;
		//	case Freq_RX_2417m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_2417m[i]);
		//		}
		//		freq = 2417;
		//	}break;
		//	case Freq_RX_2447m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_2447m[i]);
		//		}
		//		freq = 2447;
		//	}break;
		//	case Freq_RX_2477m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_2477m[i]);
		//		}
		//		freq = 2477;
		//	}break;
		//	case Freq_RX_5157m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5157m[i]);
		//		}
		//		freq = 5157;
		//	}break;
		//	case Freq_RX_5187m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5187m[i]);
		//		}
		//		freq = 5187;
		//	}break;
		//	case Freq_RX_5217m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5217m[i]);
		//		}
		//		freq = 5217;
		//	}break;
		//	case Freq_RX_5247m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5247m[i]);
		//		}
		//		freq = 5247;
		//	}break;
		//	case Freq_RX_5742m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5742m[i]);
		//		}
		//		freq = 5742;
		//	}break;
		//	case Freq_RX_5772m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5772m[i]);
		//		}
		//		freq = 5772;
		//	}break;
		//	case Freq_RX_5802m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5802m[i]);
		//		}
		//		freq = 5802;
		//	}break;
		//	case Freq_RX_5832m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5832m[i]);
		//		}
		//		freq = 5832;
		//	}break;
		//	case Freq_RX_5862m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5862m[i]);
		//		}
		//		freq = 5862;
		//	}break;

		//	case Freq_RX_433m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_433m[i]);
		//		}
		//		freq = 433;
		//	}break;
		//	case Freq_RX_842m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_842m[i]);
		//		}
		//		freq = 842;
		//	}break;
		//	case Freq_RX_915m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_915m[i]);
		//		}
		//		freq = 915;
		//	}break;
	case Freq_RX_2420m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2420m[i]);
		}
		freq = 2420;
	}
	break;
		//	case Freq_RX_837m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_837m[i]);
		//		}
		//		freq = 837;
		//	}break;
		//	case Freq_RX_2406m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_2406m[i]);
		//		}
		//		freq = 2406;
		//	}break;
	case Freq_RX_2460m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2460m[i]);
		}
		freq = 2460;
	}
	break;
		//	case Freq_RX_5160m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5160m[i]);
		//		}
		//		freq = 5160;
		//	}break;
		//	case Freq_RX_5200m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5200m[i]);
		//		}
		//		freq = 5200;
		//	}break;
		//	case Freq_RX_5240m:
		//	{
		//		for (i = 0; i < cnt; i++)
		//		{
		//			SetAd(Ad9361FreqConfig_RX_5240m[i]);
		//		}
		//		freq = 5240;
		//	}break;
	case Freq_RX_5745m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5745m[i]);
		}
		freq = 5745;
	}
	break;
	case Freq_RX_5785m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5785m[i]);
		}
		freq = 5785;
	}
	break;
	case Freq_RX_5825m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5825m[i]);
		}
		freq = 5825;
	}
	break;
	case Freq_RX_5865m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5865m[i]);
		}
		freq = 5865;
	}
	break;
	case Freq_MAX_CNT:
	default:
		break;
	}

	if (GetAd9361GainCutoverFlag() == 1)
	{
		GainCnt++;
		if (GainCnt >= Freq_MAX_CNT)
		{
			GainCnt = 0;
			GainCutoverIndex++;
			if (GainCutoverIndex > 1)
			{
				GainCutoverIndex = 0;
			}
			SetAd9361Gain(Ad9361GainBuf[GainCutoverIndex]);
			LOG_DEBUG("Gain:%02X \n\r", Ad9361GainBuf[GainCutoverIndex]);
		}
	}
	return freq;
}

/* Gets the RX frequency. */
uint32_t get_rx_freq(uint32_t freqItem)
{
	uint32_t freq = 0;
	switch (freqItem)
	{
		//	case Freq_RX_875m:
		//	{
		//		freq = 875;
		//	}break;
		//	case Freq_RX_2417m:
		//	{
		//		freq = 2417;
		//	}break;
		//	case Freq_RX_2447m:
		//	{
		//		freq = 2447;
		//	}break;
		//	case Freq_RX_2477m:
		//	{
		//		freq = 2477;
		//	}break;
		//	case Freq_RX_5157m:
		//	{
		//		freq = 5157;
		//	}break;
		//	case Freq_RX_5187m:
		//	{
		//		freq = 5187;
		//	}break;
		//	case Freq_RX_5217m:
		//	{
		//		freq = 5217;
		//	}break;
		//	case Freq_RX_5247m:
		//	{
		//		freq = 5247;
		//	}break;
		//	case Freq_RX_5742m:
		//	{
		//		freq = 5742;
		//	}break;
		//	case Freq_RX_5772m:
		//	{
		//		freq = 5772;
		//	}break;
		//	case Freq_RX_5802m:
		//	{
		//		freq = 5802;
		//	}break;
		//	case Freq_RX_5832m:
		//	{
		//		freq = 5832;
		//	}break;
		//	case Freq_RX_5862m:
		//	{
		//		freq = 5862;
		//	}break;

		//	case Freq_RX_433m:
		//	{
		//		freq = 433;
		//	}break;
		//	case Freq_RX_837m:
		//	{
		//		freq = 837;
		//	}break;
		//	case Freq_RX_2406m:
		//	{
		//		freq = 2406;
		//	}break;
		//	case Freq_RX_842m:
		//	{
		//		freq = 842;
		//	}break;
		//	case Freq_RX_915m:
		//	{
		//		freq = 915;
		//	}break;
	case Freq_RX_2420m:
	{
		freq = 2420;
	}
	break;
	case Freq_RX_2460m:
	{
		freq = 2460;
	}
	break;
		//	case Freq_RX_5160m:
		//	{
		//		freq = 5160;
		//	}break;
		//	case Freq_RX_5200m:
		//	{
		//		freq = 5200;
		//	}break;
		//	case Freq_RX_5240m:
		//	{
		//		freq = 5240;
		//	}break;
	case Freq_RX_5745m:
	{
		freq = 5745;
	}
	break;
	case Freq_RX_5785m:
	{
		freq = 5785;
	}
	break;
	case Freq_RX_5825m:
	{
		freq = 5825;
	}
	break;
	case Freq_RX_5865m:
	{
		freq = 5865;
	}
	break;
	case Freq_MAX_CNT:
	default:
		break;
	}

	return freq;
}

void ad9361_generate_fast_lock_profile(uint8_t profile_num, uint8_t *reg_value_list)
{
	u16 row, i;
	u8 Ad9361Config[][3] = {
		{0x82, 0x9a, 0x03},
		{0x82, 0x9c, 0x00 + profile_num},
		{0x82, 0x9d, reg_value_list[1]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x01 + profile_num},
		{0x82, 0x9d, reg_value_list[2]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x02 + profile_num},
		{0x82, 0x9d, reg_value_list[3]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x03 + profile_num}, // Synthesizer Fractional Word[15:8] 0x274[D7:D0]
		{0x82, 0x9d, reg_value_list[4]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x04 + profile_num}, // Synthesizer Fractional Word[22:16] 0x275[D6:D0]
		{0x82, 0x9d, reg_value_list[5] & 0x7f},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x05 + profile_num}, // VCO Bias Ref[2:0] shift left by 4;  0x282[D2:D0]
		{0x82, 0x9d, ((reg_value_list[14] & 0x7) << 4) + (reg_value_list[9] & 0xf)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x06 + profile_num}, // VCO Varactor[3:0]; 0x279[D3:D0]    VCO Bias Tcf[1:0] shift left by 6;  0x282[D4:D3]
		{0x82, 0x9d, (((reg_value_list[14] & 0x18) >> 3) << 6) + (reg_value_list[10] & 0x3f)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x07 + profile_num}, // Charge Pump Current (Init)[5:0]; Set per Init ; Charge Pump Current[5:0]; 0x27B[D5:D0]
		{0x82, 0x9d, reg_value_list[10] & 0x3f},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x08 + profile_num}, // Loop Filter R3[3:0] shift left by 4; 0x280[D3:D0]
		{0x82, 0x9d, ((reg_value_list[13] & 0xf) << 4) + (reg_value_list[13] & 0xf)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x09 + profile_num}, // Loop Filter R3 (Init)[3:0]; Set per Init; Loop Filter C3[3:0] shift left by 4; 0x27F[D3:D0]
		{0x82, 0x9d, ((reg_value_list[12] & 0xf) << 4) + (reg_value_list[12] & 0xf)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0A + profile_num}, // Loop Filter C3 (Init)[3:0] ;Set per Init; Loop Filter C1[3:0] shift left by 4; 0x27E[D3:D0]
		{0x82, 0x9d, ((reg_value_list[11] & 0xf) << 4) + (reg_value_list[11] >> 4)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0B + profile_num}, // Loop Filter C2[3:0] shift right by 4;0x27E[D7:D4]); Loop Filter R1[3:0]; 0x27F[D7:D4]
		{0x82, 0x9d, (reg_value_list[12] & 0xf0) + (reg_value_list[12] >> 4)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0C + profile_num}, // Loop Filter R1 (Init)[3:0]; Set per Init N calculation;VCO Varactor Reference Tcf[2:0]; 0x290[D6:D4]
		{0x82, 0x9d, (reg_value_list[15] & 0x70) + (reg_value_list[0] >> 4)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0D + profile_num}, // Rx VCO Divider[3:0]; 0x005[D7:D4];  VCO Cal Offset[3:0] shift left by 1;0x278[D6:D3]
		{0x82, 0x9d, ((reg_value_list[8] & 0x78) << 1) + (reg_value_list[16] & 0xf)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0E + profile_num}, // VCO Varactor Reference[3:0]; 0x291[D3:D0]; Force VCO Tune[7:0]; 0x277[D7:D0]
		{0x82, 0x9d, reg_value_list[7]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0F + profile_num}, // Force ALC word[6:0] shift left by 1; 0x276[D6:D0] Force VCO Tune[8]; 0x278[D0]
		{0x82, 0x9d, ((reg_value_list[6] & 0x7f) << 1) + (reg_value_list[8] & 0x1)},
		{0x82, 0x9f, 0x03},
	};
	row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
	for (i = 0; i < row; i++)
	{
		SetAd(Ad9361Config[i]);
	}
}

void enable_fast_lock_profile()
{
	u16 row, i;
	u8 Ad9361Config[][3] = {
		{0x82, 0x9B, 0x00},
		{0x82, 0x9a, 0x03},
		{0x82, 0x9f, 0x00},
		{0x80, 0x15, 0x0F},
		{0x82, 0x70, 0x55},
	};
	row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
	for (i = 0; i < row; i++)
	{
		SetAd(Ad9361Config[i]);
	}
}

#define HOP_BY_GPIO

static void fre_hop_handle(void *param)
{
	static uint8_t hopIndex = 0;

	XGpio_DiscreteWrite(&sGpio, 1, hopIndex << 1);
	hopIndex++;
	if (uGps915mHitFlag == Freq_TX_GPS_915m)
	{
		if (hopIndex >= fre_max)
		{
			hopIndex = 0;
		}
	}
	else if (uGps915mHitFlag == Freq_TX_915m)
	{
		if (hopIndex > fre_915m_addr)
		{
			hopIndex = 0;
		}
	}
	else // Freq_TX_GPS
	{
		if (hopIndex >= fre_max)
		{
			hopIndex = fre_1572m_addr;
		}
	}
}

int32_t Ad9361_fre_hop_init(void)
{
	int status;

	//	uint8_t Ad9361Config[3] = {0x00, 0x05, 0xff};
	//	for(uint8_t i=0;i<17;i++){
	//		Ad9361Config[0] = (TX_read_reg_addr_list[i] & 0x0F00) >> 8;
	//		Ad9361Config[1] = (TX_read_reg_addr_list[i] & 0x00FF);
	//		SetAd(Ad9361Config);
	//	}

	ad9361_cfg_tx_init(); // debug
	XGpio_Initialize(&sGpio, XPAR_AXI_GPIO_1_DEVICE_ID);
	status = XScuGic_Connect(&xInterruptController,
							 AD9361_FRE_HOP_TIME,
							 (Xil_ExceptionHandler)fre_hop_handle,
							 NULL);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	IntcTypeSetup(&xInterruptController, AD9361_FRE_HOP_TIME, 0x03);

	//	Ad9361_fre_hop_start();

	return 0;
}

int32_t Ad9361_fre_hop_start(void)
{
	XScuGic_Enable(&xInterruptController, AD9361_FRE_HOP_TIME);

	return 0;
}

int32_t Ad9361_fre_hop_stop(void)
{
	XScuGic_Disable(&xInterruptController, AD9361_FRE_HOP_TIME);

	return 0;
}

int32_t Ad9361_fre_hop_set(int32_t rfRang)
{
	ad9361_generate_fast_lock_profile(fre_433m_addr << 4, reg_value_list_433m);
	ad9361_generate_fast_lock_profile(fre_842m_addr << 4, reg_value_list_842m);
	ad9361_generate_fast_lock_profile(fre_915m_addr << 4, reg_value_list_915m);
	//	ad9361_generate_fast_lock_profile(fre_1931m_addr << 4, reg_value_list_1931m);
		ad9361_generate_fast_lock_profile(fre_1433m_addr << 4, reg_value_list_1433m);
	//	ad9361_generate_fast_lock_profile(fre_1187m_addr << 4, reg_value_list_1187m);
	//	ad9361_generate_fast_lock_profile(fre_1212m_addr << 4, reg_value_list_1212m);
	//	ad9361_generate_fast_lock_profile(fre_1253m_addr << 4, reg_value_list_1253m);
	//	ad9361_generate_fast_lock_profile(fre_1265m_addr << 4, reg_value_list_1265m);
	ad9361_generate_fast_lock_profile(fre_1572m_addr << 4, reg_value_list_1572m);
	ad9361_generate_fast_lock_profile(fre_1597m_addr << 4, reg_value_list_1597m);
	enable_fast_lock_profile();
	uGps915mHitFlag = rfRang;

	return 0;
}

void SetAD9361Gpio(uint32_t value)
{
	XGpio_DiscreteWrite(&sGpio, 1, value);
}

void SetAd9361Gain(uint8_t value)
{
	u8 Ad9361Config[3] = {0x81, 0x09, 0x00};
	u8 Ad9361Config2[3] = {0x81, 0x0C, 0x00};
	Ad9361Config[2] = value;
	Ad9361Config2[2] = value;
	SetAd(Ad9361Config);
	SetAd(Ad9361Config2);
}

static uint8_t Ad9361GainCutoverFlag = 0;
void SetAd9361GainCutoverFlag(uint8_t value)
{
	Ad9361GainCutoverFlag = value;
}
uint8_t GetAd9361GainCutoverFlag(void)
{
	return Ad9361GainCutoverFlag;
}
void SetAd9361GainCutoverValue(uint8_t value1, uint8_t value2)
{
	Ad9361GainBuf[0] = value1;
	Ad9361GainBuf[1] = value2;
}

void SetAd9361GainAGC(uint8_t value)
{
	u16 row, i;
	static u8 Ad9361Config[][3] = {
		//************************************************************
		// Setup Rx AGC Fast AttackRegisters
		//************************************************************
		{0x80, 0x22, 0x0A}, // SPIWrite	022,0A	// AGC Fast Attack Gain Lock Delay
		{0x80, 0xFA, 0xE5}, // SPIWrite	0FA,E5	// Gain Control Mode Select
		{0x80, 0xFB, 0x08}, // SPIWrite	0FB,08	// Gain Control Config
		{0x80, 0xFC, 0x23}, // SPIWrite	0FC,23	// ADC Overrange Sample Size
		{0x80, 0xFD, 0x4C}, // SPIWrite	0FD,4C	// Max Full/LMT Gain Table Index
		{0x80, 0xFE, 0x44}, // SPIWrite	0FE,44	// Peak Overload Wait Time
		{0x81, 0x00, 0x6F}, // SPIWrite	100,6F	// Dig Gain: Step Size & Max
		{0x81, 0x01, 0x0A}, // SPIWrite	101,0A	// AGC Lock Level
		{0x81, 0x03, 0x08}, // SPIWrite	103,08	// Large LMT or Step 3 Size
		{0x81, 0x04, 0x2F}, // SPIWrite	104,2F	// ADC Small Overload Threshold
		{0x81, 0x05, 0x3A}, // SPIWrite	105,3A	// ADC Large Overload Threshold
		{0x81, 0x06, 0x22}, // SPIWrite	106,22	// Overload Step Sizes
		{0x81, 0x07, 0x2B}, // SPIWrite	107,2B	// Small LMT Overload Threshold
		{0x81, 0x08, 0x31}, // SPIWrite	108,31	// Large LMT Overload Threshold
		{0x81, 0x09, 0x4C}, // SPIWrite	109,4C	// State 5 Power Measurement MSB
		{0x81, 0x0A, 0x58}, // SPIWrite	10A,58	// State 5 Power Measurement LSBs
		{0x81, 0x0B, 0x00}, // SPIWrite	10B,00	// Rx1 Force Digital Gain
		{0x81, 0x0E, 0x00}, // SPIWrite	10E,00	// Rx2 Force Digital Gain
		{0x81, 0x10, 0x00}, // SPIWrite	110,00	// AGC Fast Attack Config
		{0x81, 0x11, 0x0A}, // SPIWrite	111,0A	// Settling Delay & AGC Config
		{0x81, 0x12, 0x52}, // SPIWrite	112,52	// Post Lock Step & Energy Lost Thresh
		{0x81, 0x13, 0x4C}, // SPIWrite	113,4C	// Post Lock Step & Strong Sig Thresh
		{0x81, 0x14, 0x30}, // SPIWrite	114,30	// Low Power Threshold & ADC Ovr Ld
		{0x81, 0x15, 0x00}, // SPIWrite	115,00	// Stronger Signal Unlock Control
		{0x81, 0x16, 0x65}, // SPIWrite	116,65	// Final Overrange and Opt Gain Offset
		{0x81, 0x17, 0x08}, // SPIWrite	117,08	// Gain Inc Step & Energy Detect Cnt
		{0x81, 0x18, 0x05}, // SPIWrite	118,05	// Lock Level GAin Incr Upper Limit
		{0x81, 0x19, 0x08}, // SPIWrite	119,08	// Gain Lock Exit Count
		{0x81, 0x1A, 0x27}, // SPIWrite	11A,27	// Initial LMT Gain Limit
		{0x81, 0x1B, 0x0A}, // SPIWrite	11B,0A	// Increment Time
	};
	static u8 Ad9361Config1[][3] = {
		//************************************************************
		// Setup Rx Manual Gain Registers
		//************************************************************
		{0x80, 0xFA, 0xE0}, // SPIWrite	0FA,E0	// Gain Control Mode Select
		{0x80, 0xFB, 0x08}, // SPIWrite	0FB,08	// Table, Digital Gain, Man Gain Ctrl
		{0x80, 0xFC, 0x23}, // SPIWrite	0FC,23	// Incr Step Size, ADC Overrange Size
		{0x80, 0xFD, 0x4C}, // SPIWrite	0FD,4C	// Max Full/LMT Gain Table Index
		{0x80, 0xFE, 0x44}, // SPIWrite	0FE,44	// Decr Step Size, Peak Overload Time
		{0x81, 0x00, 0x6F}, // SPIWrite	100,6F	// Max Digital Gain
		{0x81, 0x04, 0x2F}, // SPIWrite	104,2F	// ADC Small Overload Threshold
		{0x81, 0x05, 0x3A}, // SPIWrite	105,3A	// ADC Large Overload Threshold
		{0x81, 0x07, 0x2B}, // SPIWrite	107,2B	// Small LMT Overload Threshold
		{0x81, 0x08, 0x31}, // SPIWrite	108,31	// Large LMT Overload Threshold
		{0x81, 0x09, 0x3C}, // SPIWrite	109,0A	// Rx1 Full/LMT Gain Index
		{0x81, 0x0A, 0x58}, // SPIWrite	10A,58	// Rx1 LPF Gain Index
		{0x81, 0x0B, 0x00}, // SPIWrite	10B,00	// Rx1 Digital Gain Index
		{0x81, 0x0C, 0x3C}, // SPIWrite	10C,0A	// Rx2 Full/LMT Gain Index
		{0x81, 0x0D, 0x18}, // SPIWrite	10D,18	// Rx2 LPF Gain Index
		{0x81, 0x0E, 0x00}, // SPIWrite	10E,00	// Rx2 Digital Gain Index
		{0x81, 0x14, 0x30}, // SPIWrite	114,30	// Low Power Threshold
		{0x81, 0x1A, 0x27}, // SPIWrite	11A,27	// Initial LMT Gain Limit
		{0x80, 0x81, 0x00}, // SPIWrite	081,00	// Tx Symbol Gain Control
	};
	if (value == 1)
	{
		row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
		for (i = 0; i < row; i++)
		{
			SetAd(Ad9361Config[i]);
		}
	}
	else if (value == 0)
	{
		row = sizeof(Ad9361Config1) / sizeof(Ad9361Config1[0]);
		for (i = 0; i < row; i++)
		{
			SetAd(Ad9361Config1[i]);
		}
	}
	else if (value == 2)
	{
		uint8_t dat = ad9361_spi_read_byte(0x10C);
		LOG_DEBUG("Gain: %d\r\n", dat);
	}
}
