#include <stdint.h>

#include "flash.h"
#include "xqspips.h"
#include "xil_printf.h"

#define QSPI_DEVICE_ID		XPAR_XQSPIPS_0_DEVICE_ID

/*
 * The following constants define the commands which may be sent to the FLASH device.
 */
#define WRITE_STATUS_CMD	0x01
#define WRITE_CMD			0x02
#define READ_CMD			0x03
#define WRITE_DISABLE_CMD	0x04
#define READ_STATUS_CMD		0x05
#define WRITE_ENABLE_CMD	0x06
#define FAST_READ_CMD		0x0B
#define DUAL_READ_CMD		0x3B
#define QUAD_READ_CMD		0x6B
#define BULK_ERASE_CMD		0xC7
#define	SEC_ERASE_CMD		0xD8
#define READ_ID_CMD			0x9F

#define COMMAND_OFFSET		0 /* FLASH instruction */
#define ADDRESS_1_OFFSET	1 /* MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2 /* Mid byte of address to read or write */
#define ADDRESS_3_OFFSET	3 /* LSB byte of address to read or write */
#define DATA_OFFSET			4 /* Start of Data for Read/Write */
#define DUMMY_OFFSET		5 /* Dummy byte offset for fast, dual and quad reads */
#define DUMMY_SIZE			1 /* Number of dummy bytes for fast, dual and quad reads */
#define RD_ID_SIZE			4 /* Read ID command + 3 bytes ID response */
#define BANK_SEL_SIZE		2 /* BRWR or EARWR command + 1 byte bank value */
#define WRITE_ENABLE_CMD_SIZE	1 /* WE command */
/*
 * The following constants specify the extra bytes which are sent to the
 * FLASH on the QSPI interface, that are not data, but control information
 * which includes the command and address
 */
#define OVERHEAD_SIZE		4

static XQspiPs QspiInstanceApp;
static XQspiPs *QspiInstanceAppPtr;

/*
 * The following variables are used to read and write to the eeprom and they
 * are global to avoid having large buffers on the stack
 */
uint8_t ReadBuffer[DATA_SIZE + DATA_OFFSET + DUMMY_SIZE];
uint8_t WriteBuffer[DATA_SIZE + DATA_OFFSET + DUMMY_SIZE];
uint32_t QspiFlashSize;
uint32_t QspiFlashManu;


uint32_t QspiInitApp(void)
{
	XQspiPs_Config *QspiConfig;
	int Status;

	QspiInstanceAppPtr = &QspiInstanceApp;

	/*
	 * Set up the base address for access
	 */
//	FlashReadBaseAddress = XPS_QSPI_LINEAR_BASEADDR;

	/*
	 * Initialize the QSPI driver so that it's ready to use
	 */
	QspiConfig = XQspiPs_LookupConfig(QSPI_DEVICE_ID);
	if (NULL == QspiConfig) {
		return XST_FAILURE;
	}

	Status = XQspiPs_CfgInitialize(QspiInstanceAppPtr, QspiConfig,
					QspiConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	QspiInstanceAppPtr->Config.InputClockHz = XPAR_PS7_QSPI_0_QSPI_CLK_FREQ_HZ;

	return XST_SUCCESS;
}

/******************************************************************************
*
* This function reads from the serial FLASH connected to the QSPI interface.
*
* @param	Address contains the address to read data from in the FLASH.
* @param	ByteCount contains the number of bytes to read.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
int32_t flash_memory_readb(uint32_t Address, uint8_t *data, uint32_t ByteCount)
{
	// Setup the write command with the specified address and data for the FLASH
	uint32_t Status = XST_SUCCESS;

	WriteBuffer[COMMAND_OFFSET]   = XQSPIPS_FLASH_OPCODE_NORM_READ;
	WriteBuffer[ADDRESS_1_OFFSET] = (uint8_t)((Address & 0xFF0000) >> 16);
	WriteBuffer[ADDRESS_2_OFFSET] = (uint8_t)((Address & 0xFF00) >> 8);
	WriteBuffer[ADDRESS_3_OFFSET] = (uint8_t)(Address & 0xFF);

	/*
	 * Send the read command to the FLASH to read the specified number
	 * of bytes from the FLASH, send the read command and address and
	 * receive the specified number of bytes of data in the data buffer
	 */
	Status = XQspiPs_PolledTransfer(QspiInstanceAppPtr, WriteBuffer, ReadBuffer,
			ByteCount + OVERHEAD_SIZE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	memcpy((char *)data, (char *)(ReadBuffer + DATA_OFFSET), ByteCount);

	return Status;
}

/******************************************************************************
*
* This function erases 4k bytes data of flash.
*
* @param	Address contains the address to read data from in the FLASH.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void flash_memory_erase(uint32_t eraseAddr)
{
	uint8_t WriteEnableCmd = { WRITE_ENABLE_CMD };
	uint8_t ReadStatusCmd[] = { READ_STATUS_CMD, 0 };  /* Must send 2 bytes */
	uint8_t FlashStatus[2];

	XQspiPs_PolledTransfer(QspiInstanceAppPtr, &WriteEnableCmd, NULL,
			sizeof(WriteEnableCmd));

	WriteBuffer[COMMAND_OFFSET]   = XQSPIPS_FLASH_OPCODE_BE_4K;
	WriteBuffer[ADDRESS_1_OFFSET] = (uint8_t)((eraseAddr & 0xFF0000) >> 16);
	WriteBuffer[ADDRESS_2_OFFSET] = (uint8_t)((eraseAddr & 0xFF00) >> 8);
	WriteBuffer[ADDRESS_3_OFFSET] = (uint8_t)(eraseAddr & 0xFF);
    XQspiPs_PolledTransfer(QspiInstanceAppPtr, WriteBuffer, ReadBuffer, OVERHEAD_SIZE);

	/*
	 * Wait for the erase command to the Flash to be completed
	 */
	while (1) {
		/*
		 * Poll the status register of the device to determine
		 * when it completes, by sending a read status command
		 * and receiving the status byte
		 */
		XQspiPs_PolledTransfer(QspiInstanceAppPtr, ReadStatusCmd,
					FlashStatus,
					sizeof(ReadStatusCmd));

		/*
		 * If the status indicates the write is done, then stop
		 * waiting; if a value of 0xFF in the status byte is
		 * read from the device and this loop never exits, the
		 * device slave select is possibly incorrect such that
		 * the device status is not being read
		 */
		if ((FlashStatus[1] & 0x01) == 0) {
			break;
		}
	}
}

/******************************************************************************
*
* This function erases all data of flash.
*
* @param	none.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void bulkEraseQspiFlash(void)
{
	uint8_t WriteEnableCmd = { WRITE_ENABLE_CMD };
	uint8_t ReadStatusCmd[] = { READ_STATUS_CMD, 0 };  /* Must send 2 bytes */
	uint8_t FlashStatus[2];

	/*
	 * Send the write enable command to the Flash so that it can be
	 * written to, this needs to be sent as a separate transfer
	 * before the erase
	 */
	XQspiPs_PolledTransfer(QspiInstanceAppPtr, &WriteEnableCmd, NULL,
			  sizeof(WriteEnableCmd));

	WriteBuffer[COMMAND_OFFSET] = BULK_ERASE_CMD;
	XQspiPs_PolledTransfer(QspiInstanceAppPtr, WriteBuffer, NULL, WRITE_ENABLE_CMD_SIZE);

	/*
	 * Wait for the erase command to the Flash to be completed
	 */
	while (1) {
		/*
		 * Poll the status register of the device to determine
		 * when it completes, by sending a read status command
		 * and receiving the status byte
		 */
		XQspiPs_PolledTransfer(QspiInstanceAppPtr, ReadStatusCmd,
					FlashStatus,
					sizeof(ReadStatusCmd));

		/*
		 * If the status indicates the write is done, then stop
		 * waiting; if a value of 0xFF in the status byte is
		 * read from the device and this loop never exits, the
		 * device slave select is possibly incorrect such that
		 * the device status is not being read
		 */
		if ((FlashStatus[1] & 0x01) == 0) {
			break;
		}
	}
}

/******************************************************************************
*
* This function writes to the serial FLASH connected to the QSPI interface.
*
* @param	Address contains the address to read data from in the FLASH.
* @param	ByteCount contains the number of bytes to write.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
int32_t flash_memory_writeb(uint32_t Address, const uint8_t *flashData, uint32_t ByteCount)
{
	// Setup the write command with the specified address and data for the FLASH
	uint32_t Status;
	uint8_t WriteEnableCmd = { WRITE_ENABLE_CMD };
	uint8_t ReadStatusCmd[] = { READ_STATUS_CMD, 0 };  /* Must send 2 bytes */
	uint8_t FlashStatus[2];
	uint8_t fill_buf[4];	// Max 3 bytes
	uint32_t fill_len = 0;

	fill_len = 4 - (ByteCount % 4);
	flash_memory_readb(Address + ByteCount, fill_buf, fill_len);

	/*
	 * Send the write enable command to the Flash so that it can be
	 * written to, this needs to be sent as a separate transfer before
	 * the write
	 */
	XQspiPs_PolledTransfer(QspiInstanceAppPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

	WriteBuffer[COMMAND_OFFSET]   = XQSPIPS_FLASH_OPCODE_PP;
	WriteBuffer[ADDRESS_1_OFFSET] = (uint8_t)((Address & 0xFF0000) >> 16);
	WriteBuffer[ADDRESS_2_OFFSET] = (uint8_t)((Address & 0xFF00) >> 8);
	WriteBuffer[ADDRESS_3_OFFSET] = (uint8_t)(Address & 0xFF);
	memcpy((char *)(WriteBuffer + DATA_OFFSET), (char *)flashData, ByteCount);

	/*
	 * XQspiPs_PolledTransfer function to transfer data at least four bytes.
	 * Padding alignment is required when writing data that is not 4-byte aligned.
	 */
	if (fill_len > 0 && fill_len < 4)
	{
		memcpy((char *)(WriteBuffer + DATA_OFFSET + ByteCount), (char *)fill_buf, fill_len);
		ByteCount += fill_len;
	}

	/*
	 * Send the read command to the FLASH to read the specified number
	 * of bytes from the FLASH, send the read command and address and
	 * receive the specified number of bytes of data in the data buffer
	 */
	Status = XQspiPs_PolledTransfer(QspiInstanceAppPtr, WriteBuffer, NULL,
			ByteCount + OVERHEAD_SIZE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait for the erase command to the Flash to be completed
	 */
	while (1) {
		/*
		 * Poll the status register of the device to determine
		 * when it completes, by sending a read status command
		 * and receiving the status byte
		 */
		XQspiPs_PolledTransfer(QspiInstanceAppPtr, ReadStatusCmd,
					FlashStatus,
					sizeof(ReadStatusCmd));

		/*
		 * If the status indicates the write is done, then stop
		 * waiting; if a value of 0xFF in the status byte is
		 * read from the device and this loop never exits, the
		 * device slave select is possibly incorrect such that
		 * the device status is not being read
		 */
		if ((FlashStatus[1] & 0x01) == 0) {
			break;
		}
	}

	return Status;
}

/******************************************************************************
*
* This function reads from the serial FLASH connected to the QSPI interface.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
uint32_t FlashReadID(void)
{
	uint32_t Status;

	/*
	 * Read ID in Auto mode.
	 */
	WriteBuffer[COMMAND_OFFSET]   = READ_ID_CMD;
	WriteBuffer[ADDRESS_1_OFFSET] = 0x00;		/* 3 dummy bytes */
	WriteBuffer[ADDRESS_2_OFFSET] = 0x00;
	WriteBuffer[ADDRESS_3_OFFSET] = 0x00;

	Status = XQspiPs_PolledTransfer(QspiInstanceAppPtr, WriteBuffer, ReadBuffer,
				RD_ID_SIZE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	printf("FlashID=0x%x 0x%x 0x%x\r\n", ReadBuffer[1],
			ReadBuffer[2],
			ReadBuffer[3]);

	/*
	 * Deduce flash make
	 */
	if (ReadBuffer[1] == MICRON_ID) {
		QspiFlashManu = MICRON_ID;
	} else if(ReadBuffer[1] == SPANSION_ID) {
		QspiFlashManu = SPANSION_ID;
	} else if(ReadBuffer[1] == WINBOND_ID) {
		QspiFlashManu = WINBOND_ID;
	} else if(ReadBuffer[1] == MACRONIX_ID) {
		QspiFlashManu = MACRONIX_ID;
	} else if(ReadBuffer[1] == ISSI_ID) {
		QspiFlashManu = ISSI_ID;
	}

	/*
	 * Deduce flash Size
	 */
	if (ReadBuffer[3] == FLASH_SIZE_ID_8M) {
		QspiFlashSize = FLASH_SIZE_8M;
	} else if (ReadBuffer[3] == FLASH_SIZE_ID_16M) {
		QspiFlashSize = FLASH_SIZE_16M;
	} else if (ReadBuffer[3] == FLASH_SIZE_ID_32M) {
		QspiFlashSize = FLASH_SIZE_32M;
	} else if (ReadBuffer[3] == FLASH_SIZE_ID_64M) {
		QspiFlashSize = FLASH_SIZE_64M;
	} else if (ReadBuffer[3] == FLASH_SIZE_ID_128M) {
		QspiFlashSize = FLASH_SIZE_128M;
	} else if (ReadBuffer[3] == FLASH_SIZE_ID_256M) {
		QspiFlashSize = FLASH_SIZE_256M;
	} else if ((ReadBuffer[3] == FLASH_SIZE_ID_512M)
			|| (ReadBuffer[3] == MACRONIX_FLASH_1_8_V_MX66_ID_512)
			|| (ReadBuffer[3] == MACRONIX_FLASH_SIZE_ID_512M)) {
		QspiFlashSize = FLASH_SIZE_512M;
	} else if ((ReadBuffer[3] == FLASH_SIZE_ID_1G)
			|| (ReadBuffer[3] == MACRONIX_FLASH_SIZE_ID_1G)) {
		QspiFlashSize = FLASH_SIZE_1G;
	} else if ((ReadBuffer[3] == FLASH_SIZE_ID_2G)
			|| (ReadBuffer[3] == MACRONIX_FLASH_SIZE_ID_2G)) {
		QspiFlashSize = FLASH_SIZE_2G;
	}

	xil_printf("Flash manufacturer is %lu, size = %lu\r\n", QspiFlashManu, QspiFlashSize);
	return XST_SUCCESS;
}

