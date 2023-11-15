#ifndef SERVICE_SETWORKMODE_H
#define SERVICE_SETWORKMODE_H

#include <stdint.h>
#include "../../alink/alink_msg_proc.h"
#include "service_manager.h"

uint16_t Service_SetWorkMode_fun(alink_msg_t *request_msg, alink_msg_t *response_msg);
uint8_t Get_WorkMode();

#endif
