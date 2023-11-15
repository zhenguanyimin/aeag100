/*
 * UDS_Service.c
 *
 * Created on: 2021-7-10
 *      Author: A19199
 */

#include "../servers/service_manager.h"

extern const SERVICE_ENTRY Service_SystemReset;
extern const SERVICE_ENTRY Service_CtrGnssHitStart;
extern const SERVICE_ENTRY Service_GetSwitchState;
extern const SERVICE_ENTRY Service_SetHitFreq;
extern const SERVICE_ENTRY Service_SetWorkMode;
extern const SERVICE_ENTRY Service_GetWorkMode;
extern const SERVICE_ENTRY Service_SetCalibValue;
extern const SERVICE_ENTRY Service_GetCalibValue;
extern const SERVICE_ENTRY Service_SetBoardSN;
extern const SERVICE_ENTRY Service_GetBoardSN;
extern const SERVICE_ENTRY Service_GetSoftwareVersion;
extern const SERVICE_ENTRY Service_GetGNSSState;
extern const SERVICE_ENTRY Service_GetHitFreq;

uint32_t ServiceManagerBuf[] =
    {
        (uint32_t)&Service_SystemReset,
        (uint32_t)&Service_SetWorkMode,
        (uint32_t)&Service_GetWorkMode,
        (uint32_t)&Service_GetSwitchState,
        (uint32_t)&Service_CtrGnssHitStart,
        (uint32_t)&Service_SetHitFreq,
        (uint32_t)&Service_SetCalibValue,
        (uint32_t)&Service_GetCalibValue,
        (uint32_t)&Service_SetBoardSN,
        (uint32_t)&Service_GetBoardSN,
        (uint32_t)&Service_GetSoftwareVersion,
        (uint32_t)&Service_GetGNSSState,
        (uint32_t)&Service_GetHitFreq,
};

/**
 *
 * @param service_id
 * @return
 */
SERVICE_ENTRY *ServiceManager_GetEntry(uint8_t service_msgid)
{
    SERVICE_ENTRY *service_find;

    uint8_t sectionindex = 0;
    for (; sectionindex < (sizeof(ServiceManagerBuf) / sizeof(ServiceManagerBuf[0])); sectionindex++)
    {
        service_find = (SERVICE_ENTRY *)ServiceManagerBuf[sectionindex];
        if (service_msgid == service_find->id)
        {
            return service_find;
        }
    }

    return (SERVICE_ENTRY *)0;
}
