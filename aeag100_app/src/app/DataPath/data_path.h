
#ifndef __DATA_PATH_H__
#define __DATA_PATH_H__

/*========================================================================================
*                                     INCLUDE FILES
=========================================================================================*/
#include "xil_types.h"
#include "../../hal/DataTransfer/DataTransfer.h"

/*========================================================================================
*                                      LOCAL MACROS
=========================================================================================*/
#define RAW_ADC_LEN (256 * 1024)
#define RDMAP_DATA_LEN (8 * 4096) //(8 * 2048) /* length of parameter estimate result from PL per dma interrupt, origin radar rdmap length is (256 * 1024) */
#define DDR_BASE_ADDR (0x10000000)
#define ADC_BASE_ADDR (DDR_BASE_ADDR + 0x6400000)
#define RDM_BASE_ADDR (ADC_BASE_ADDR + 2 * (RAW_ADC_LEN))

/*========================================================================================
*                       LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
=========================================================================================*/
typedef struct
{
	u32 flag; // 1 for in using ,0 for idle
	u32 type;
	u32 ts;
	u32 length; // length of the pData
	u8 *pData;
} buffer_t;

typedef struct
{
	buffer_t buff1;
	buffer_t buff2;
	volatile u32 revLen;
	u32 usedId;
} DataPingPong_t;

enum
{
	BUFF_STAT_START,
	BUFF_EMPTY,
	BUFF_IN_USED,
	BUFF_FULL,
	BUFF_STAT_MAX
};

/*========================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
=========================================================================================*/

/*========================================================================================
*                                    FUNCTIONS
=========================================================================================*/
typedef void (*DataPathEvCb)(u8 *pBuf, s32 len);

s32 data_path_init(void);

s32 data_path_start(s32 type);

s32 data_path_stop(s32 type);

void data_path_out_en(s32 type);

s32 data_path_setEvCallBack(DataPathEvCb pCb); // for dectect process ,return immediately

void dma_transfer_adc_init();

void Clear_RdmapDmaDataCount();

void SetOutputSpecMat(uint8_t value);

#endif
