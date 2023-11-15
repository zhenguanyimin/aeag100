/*
 * display.h
 *
 *  Created on: 2022-12-03
 *      Author: A19199
 */
#ifndef LOGGING_H
#define LOGGING_H

enum LOGGING
{
	eLOG_VERSION_STR = 0x00,
	eLOG_AD9361_STAUTS = 0x01,
	eLOG_START_HIT_FRE,
	eLOG_END_HIT,
	eLOG_HIT_CURRENT,
	eLOG_ELECTRICITY,
	eLOG_DETECT_INFO_FRE_BW_A,
	eLOG_DETECT_POINTS_WARNIG,
	eLOG_KEY_TRIGGER,
	eLOG_DETECT_OUTSOURCE_INFO,
	eLOG_DAC_PARAM,
	eLOG_LOGGING_OUTPUT,
	eLOG_END
};
#pragma pack(1)
typedef struct sVersionStr_msg
{
	uint8_t len;
	uint8_t Str[40];
} sVersionStr_msg_t;

typedef struct sAd9361Status_msg
{
	uint8_t status;
} sAd9361Status_msg_t;

typedef struct sStartHit_msg
{
	uint32_t uTimestamp;
	uint8_t uFreItem;
	uint8_t uGpsOnFlag;
} sStartHit_msg_t;

typedef struct sHitCurrent_msg
{
	uint32_t uTimestamp;
	uint16_t uCurrent;
} sHitCurrent_msg_t;

typedef struct sEndHit_msg
{
	uint32_t uTimestamp;
} sEndHit_msg_t;

typedef struct sElectricity_msg
{
	uint8_t uElectricity;
	uint8_t uResult;
} sElectricity_msg_t;

typedef struct sDetectInfo_msg
{
	uint32_t acqTime;  /* time of signal acquisition, in us */
	uint32_t freq;	   /* signal frequency, in MHz */
	uint32_t BW;	   /* signal bandwidth, in KHz */
	uint32_t dur;	   /* signal duration, in us */
	uint32_t numTtl;   /* signal occurrence number  */
	uint32_t prt;	   /* signal repetition period*/
	int16_t amp;	   /* signal amplitude, in dB */
	uint16_t distance; /* signal distance, in m */
} sDetectInfo_msg_t;

typedef struct sDetectPointsWarnig_msg
{
	uint32_t uTimestamp;
	uint8_t uWarningLeval;
	uint8_t uDetectPoints[5];
} sDetectPointsWarnig_msg_t;

typedef struct sKeyTrigger_msg
{
	uint32_t uTimestamp;
	uint8_t IO_Status;
} sKeyTrigger_msg_t;

typedef struct sDetectOutsourceInfo_msg
{
	uint32_t uTimestamp;
	char name[20]; //名称
	char flag;	   //图传0,遥控1
	uint16_t freq; //频点，MHz
				   //	uint16_t burstTime[20];//突发起点时间，ms
				   //	uint16_t width[20];    //每包时长，ms
	uint16_t bw;   //带宽，MHz
} sDetectOutsourceInfo_msg_t;

typedef struct sDacParam_msg
{
	uint32_t Param[12];
} sDacParam_msg_t;
#pragma pack(0)

typedef struct sLogging_msg
{
	uint8_t uLogNum; /*编号*/
	union
	{
		sVersionStr_msg_t VersionStr_msg;
		sAd9361Status_msg_t Ad9361Status_msg;
		sStartHit_msg_t StartHit_msg;
		sHitCurrent_msg_t HitCurrent_msg;
		sEndHit_msg_t EndHit_msg;
		sElectricity_msg_t Electricity_msg;
		sDetectInfo_msg_t DetectInfo_msg;
		sDetectPointsWarnig_msg_t sDetectPointsWarnig_msg;
		sKeyTrigger_msg_t KeyTrigger_msg;
		sDetectOutsourceInfo_msg_t sDetectOutsourceInfo_msg;
		sDacParam_msg_t sDacParam_msg;
		uint8_t buf[50];
	};
} sLogging_msg_t;

void LoggingTask_Post(sLogging_msg_t *msg);

int32_t InitLoggingProcess_Task();

int32_t ClearLogging(void);

void LoggingTest(void);

#endif /* LOGGING_H */
