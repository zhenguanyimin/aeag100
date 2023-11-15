
#ifndef FLASH_H
#define FLASH_H

//#include "common.h"
#include "xgpio.h"
#include <stdio.h>

#define MICRON_ID		0x20
#define SPANSION_ID		0x01
#define WINBOND_ID		0xEF
#define MACRONIX_ID		0xC2

#define ISSI_ID			0x9D

#define FLASH_SIZE_ID_8M		0x14
#define FLASH_SIZE_ID_16M		0x15
#define FLASH_SIZE_ID_32M		0x16
#define FLASH_SIZE_ID_64M		0x17
#define FLASH_SIZE_ID_128M		0x18
#define FLASH_SIZE_ID_256M		0x19
#define FLASH_SIZE_ID_512M		0x20
#define FLASH_SIZE_ID_1G		0x21
#define FLASH_SIZE_ID_2G		0x22
/* Macronix size constants are different for 512M and 1G */
#define MACRONIX_FLASH_SIZE_ID_512M		0x1A
#define MACRONIX_FLASH_SIZE_ID_1G		0x1B
#define MACRONIX_FLASH_SIZE_ID_2G		0x1C
#define MACRONIX_FLASH_1_8_V_MX66_ID_512        (0x3A)

/*
 * Size in bits
 */
#define FLASH_SIZE_8M			0x0100000
#define FLASH_SIZE_16M			0x0200000
#define FLASH_SIZE_32M			0x0400000
#define FLASH_SIZE_64M			0x0800000
#define FLASH_SIZE_128M			0x1000000
#define FLASH_SIZE_256M			0x2000000
#define FLASH_SIZE_512M			0x4000000
#define FLASH_SIZE_1G			0x8000000
#define FLASH_SIZE_2G			0x10000000

/*
 * The following constants specify the max amount of data and the size of the
 * the buffer required to hold the data and overhead to transfer the data to
 * and from the FLASH.
 */
#define DATA_SIZE		4096
//#define WRITE_SIZE		1000

/* global function define */
uint32_t QspiInitApp(void);
int32_t flash_memory_readb(uint32_t Address, uint8_t *data, uint32_t ByteCount);
int32_t flash_memory_writeb(uint32_t Address, const uint8_t *flashData, uint32_t ByteCount);
void bulkEraseQspiFlash(void);
void flash_memory_erase(uint32_t eraseAddr);
uint32_t FlashReadID(void);

#endif /* FLASH_H */
