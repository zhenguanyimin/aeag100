#ifndef _FLASH_MMAP_H_
#define _FLASH_MMAP_H_

/* the memory mapping address for flash */
//#define FLASH_MMAP_FIRMWARE_BASE    (0x300000)

/**
 * Flash partition table
 * | PartitionName  | PartitionSize        | AddressBegin  | AddressEnd  | Note |
 *   header           0x00004000 (16KB)      0x00000000      0x00003FFF
 *   dbg_cert         0x00004000 (16KB)      0x00004000      0x00007FFF    unused
 *   header_bk        0x00008000 (32KB)      0x00008000      0x0000FFFF    unused
 *   antenna_info     0x00010000 (64KB)      0x00010000      0x0001FFFF
 *   bootloader_cal   0x00010000 (64KB)      0x00020000      0x0002FFFF    unused
 *   application      0x00100000 (1MB)       0x00030000      0x0012FFFF
 *   angle_calib_info 0x00010000 (64KB)      0x00130000      0x0013FFFF
 *   ota_info         0x00010000 (64KB)      0x00140000      0x0014FFFF
 *   product_info     0x00010000 (64KB)      0x00150000      0x0015FFFF
 *   bootloader_aut   0x00040000 (256KB)     0x00160000      0x0019FFFF
 *   rsv_01           0x00050000 (320KB)     0x001A0000      0x001EFFFF    unused
 *   ef_env           0x00010000 (64KB)      0x001F0000      0x001FFFFF
 *
 *   application_bk   0x00100000 (1MB)       0x00200000      0x002FFFFF    unused
 *   rsv_02           0x00100000 (1MB)       0x00300000      0x003FFFFF    unused
 */

/* If you want change the start address of "FLASH_HEADER_BASE" or "FLASH_BOOT_BASE" or "FLASH_FIRMWARE_BASE",
   Don't forget to change the value in "settings" file under "C:\Calterah_downloader" accordingly */
#define FLASH_BOOT_BASE_AUTEL       (0x000000)  // 0x0000_0000 -> 0x00BF_FFFF 12MB
#define FLASH_BOOT_SIZE_AUTEL       (0xC00000)
#define FLASH_RESERVED_BASE         (0xC00000)  // 0x00C0_0000 -> 0x00CF_FFFF 1MB
#define FLASH_RESERVED_SIZE         (0x100000)
#define FLASH_EF_ENV_BASE           (0xD00000)  // 0x00D0_0000 -> 0x00DF_FFFF 1MB
#define FLASH_EF_ENV_SIZE           (0x100000)
#define FLASH_UPGADE_IMAGE_BASE     (0xE00000)  // 0x00E0_0000 -> 0x00FF_FFFF 2MB
#define FLASH_UPGADE_IMAGE_SIZE     (0x200000)

#define FLASH_EF_LOG_SIZE           (0x80000)

#define FLASH_BOOT_BASE             FLASH_BOOT_BASE_AUTEL

#define FLASH_ERASE_MIN_SIZE        (0x001000)  // 4k
/*
 * Size in bits
 */
#define FLASH_SIZE_32MB					0x2000000 	// The 16M high of flash is not accessible currently
#define FLASH_VALIED_SIZE_16MB			0x1000000
#define FLASH_VALIED_SECTION			0x1000000
#define FLASH_PAGE_SIZE					0x100		// 256

#endif /* _FLASH_MMAP_H_ */
