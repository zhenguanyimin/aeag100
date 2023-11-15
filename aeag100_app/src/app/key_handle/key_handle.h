/*
 * hit_process_task.h
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
#ifndef KEY_HANDLE_H
#define KEY_HANDLE_H

enum KEY_TASK
{
	eKEY_TASK_NONE = 0x00,
	eKEY_TASK_HIT = 0x01,
	eKEY_TASK_SET = 0x02,
	eKEY_TASK_ROTATE_CLOCKWISE = 0x03,
	eKEY_TASK_ROTATE_REVERSE = 0x04,
	eKEY_TASK_ROTATE_SW = 0x05,
};

enum SHOW_STRIKE
{
	eSHOW = 0x01,
	eSTRIKE = 0x02,
	eSHOW_STRIKE = 0x03,
};

enum FREQ_SELE
{
	eFREQ_SELE_ALL = 0x00,
	eFREQ_SELE_2G = 0x01,
	eFREQ_SELE_2_4G = 0x02,
	eFREQ_SELE_4_6G = 0x03,
	eFREQ_SELE_2G_2_4G = 0x04,
	eFREQ_SELE_2_4G_4_6G = 0x05,
	eFREQ_SELE_2G_4_6G = 0x06,
	eFREQ_SELE_MAX,
	eFREQ_CLOSE_HIT = 0xFF
};

int32_t InitKeyHandle_Task();
void KeyHandleTask_Post(uint32_t flag);
uint8_t GetStrikeFreqItem(void);
void StrikeFreqOutputCtrl(uint8_t StrikeFreq, uint8_t mode);

#endif /* KEY_HANDLE_H */
