#ifndef SERVICE_CTRGNSSHITSTART_H
#define SERVICE_CTRGNSSHITSTART_H

#include <stdint.h>
#include "../../alink/alink_msg_proc.h"
#include "service_manager.h"

void Check_WatchStartHit();
uint16_t Service_CtrGnssHitStart_fun(alink_msg_t *request_msg, alink_msg_t *response_msg);
void Set_switchHitState(uint8_t value);
uint8_t Get_switchHitState();

#endif
