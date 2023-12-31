/*
 * UDS_Server.h
 *
 * Created on: 2022-9-20
 *      Author: A19199
 */

#ifndef WATCH_SRV_H_
#define WATCH_SRV_H_

#include <stdbool.h>
#include <stdint.h>
#include "servers/service_manager.h"
#include "../alink/alink_msg_proc.h"

#define WATCH_UARTLITE_NUM 0

void Alink_Server_Process(alink_msg_t *request_msg, alink_msg_t *response_msg);
int32_t WatchAdp_Init();

#endif /* WATCH_SRV_H_ */
