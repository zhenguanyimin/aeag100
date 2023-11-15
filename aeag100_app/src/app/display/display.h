/*
 * display.h
 *
 *  Created on: 2022-09-20
 *      Author: A19199
 */
#ifndef DISPLAY_H
#define DISPLAY_H

typedef struct sDisplay_msg
{
	uint16_t length; /*drone number 编号*/
	uint32_t *buf;
} sDisplay_msg_t;

void DisplayTask_Post(sDisplay_msg_t *msg);

int32_t InitDisplayProcess_Task();

#endif /* DISPLAY_H */
