/*
 * detect_process_task.h
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
#ifndef DETECT_PROCESS_TASK_H
#define DETECT_PROCESS_TASK_H
#include <stdio.h>
#include "../../inc/common_define.h"

void DetectProcessTask_Post(uint32_t bufaddr);

int32_t InitDetectProcess_Task();
int32_t StartTimerWaringTimeout(void);
void ClearUAVWarnSlideWinInfo(void);

#endif /* DETECT_PROCESS_TASK_H */
