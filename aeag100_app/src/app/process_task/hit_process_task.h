/*
 * hit_process_task.h
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
#ifndef HIT_PROCESS_TASK_H
#define HIT_PROCESS_TASK_H

enum HIT_EVENT
{
	eEVENT_NONE = 0x00,
	eEVENT_AUTO_HIT = 0x01,
	eEVENT_MANUAL_HIT = 0x02,
	eEVENT_CLOSE_HIT = 0x03,
	eEVENT_GNSS_HIT = 0x04,
	eEVENT_CLOSE_GNSS_HIT = 0x05,
	eEVENT_WARNING_LEVAL1 = 0x06,
	eEVENT_WARNING_LEVAL2 = 0x07,
	eEVENT_WARNING_LEVAL3 = 0x08,
	eEVENT_NO_WARNING = 0x09,
	eEVENT_NO_WARNING_AND_REMAIN,
};

enum WORK_MODE
{
	eWORK_MODE_DEWECT = 0x00,
	eWORK_MODE_HIT = 0x01,
	eWORK_MODE_DIAGNOSTIC_TEST = 0x02,
	eWORK_MODE_SLEEP = 0x03,
	eWORK_MODE_FAULT = 0x04,
};

void HitProcessTask_Post(uint32_t flag);

int32_t InitHitProcess_Task();

void SetSysWorkingMode(uint8_t mode);
uint8_t GetSysWorkingMode(void);

#endif /* HIT_PROCESS_TASK_H */
