/*
 * UDS_Service.h
 *
 * Created on: 2021-7-10
 *      Author: A19199
 */

#ifndef UDS_SERVICE_H_
#define UDS_SERVICE_H_

#include <stdint.h>

#include "../../alink/alink_msg_proc.h"

enum UDS_ServiceID
{
	//	eaeg -> Watch
	CMD_ON_OFF_SCREEN = 0xE0,
	CMD_USER_STRING = 0xE1,
	CMD_SPECI_LOCATION_STR = 0xE2,
	CMD_CLEAR_STR = 0xE3,
	CMD_ELECTRIC = 0xE4,
	CMD_WARNING = 0xE5,
	CMD_C2_LOCATION_INFO = 0xE6,
	CMD_SPIN_BUTTON = 0xE7,
	CMD_SET_BUTTON = 0xE8,

	//	Watch -> eaeg
	CMD_ADD_CFG_FPR = 0xC0,
	CMD_UPDATE_LIB = 0xC1,
	CMD_GET_LIB = 0xC2,
	CMD_SYSTEM_RESET = 0xC3,
	CMD_FACTORY_RESET = 0xC4,

	//	Watch -> eaeg Firmware upgrade
	CMD_HEARBEAT = 0xD0,
	CMD_SET_SN = 0xD4,
	CMD_GET_SN = 0xD5,
	CMD_GET_APP_FLASHSIZE = 0xD6,
	CMD_REQUEST_UPGRADE = 0xD7,
	CMD_ERASE_APP_FLASH = 0xD8,
	CMD_DOWNLOADING = 0xD9,
	CMD_GET_VERSIONS = 0xDB,
	CMD_CRC_CHECK = 0xDC,

	// pc -> eaeg
	CMD_SET_WORKMODE = 0x60,
	CMD_GET_WORKMODE = 0x61,
	CMD_GET_SWITCHSTATE = 0x62,
	CMD_CTR_GNSS_HIT_START = 0x63,
	CMD_SET_HIT_FREQ = 0x64,

	CMD_SET_CALIBVALUE = 0x65,
	CMD_GET_CALIBVALUE = 0x66,

	CMD_GET_HIT_FREQ = 0x68,
	CMD_GET_GNSS_STATE = 0x69,

	CMD_SET_BOARD_SN = 0x80,
	CMD_GET_BOARD_SN = 0x81,
	CMD_GET_SOFTWARE_VERSION = 0x82,

};

/**
 *
 */
typedef struct SERVICE_ENTRY_
{
	uint8_t id;
	/**
	 * 服务入口
	 * @param recvBuffer 请求APD
	 * @param recvLength APD 长度
	 * @param sendBuffer 响应APD缓存
	 * @return response length
	 */
	uint16_t (*entry)(alink_msg_t *request_msg, alink_msg_t *response_msg);
} SERVICE_ENTRY;

SERVICE_ENTRY *ServiceManager_GetEntry(uint8_t service_id);

#endif /* UDS_SERVICE_H_ */
