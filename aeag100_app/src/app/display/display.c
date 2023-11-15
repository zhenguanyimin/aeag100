/*
 * display.c
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"

#include "../../cfg/aeag_config.h"
static QueueHandle_t xDisplayQueue;

void DisplayTask_Post(sDisplay_msg_t *msg)
{
	if (xDisplayQueue != NULL)
	{
		xQueueSend(xDisplayQueue, msg, 10);
	}
}

static int32_t DisplayTask_Pend(sDisplay_msg_t *msg, uint32_t timeout)
{

	int32_t result = -1;

	if (xDisplayQueue != NULL)
	{
		if (xQueueReceive(xDisplayQueue, msg, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void DisplayProcess_Task(void *p_arg)
{
	sDisplay_msg_t msg;
	while (1)
	{
		if (DisplayTask_Pend(&msg, 1000) == 0)
		{
		}
	}
}

int32_t InitDisplayProcess_Task()
{
	int32_t retVal = -1;

	xDisplayQueue = xQueueCreate(5, sizeof(sDisplay_msg_t));

	if (xTaskCreate(DisplayProcess_Task, "DisplayProcess_Task", TASK_STACK_SIZE_DISPLAYPROCESS, NULL, TASK_PRI_DISPLAYPROCESS, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
