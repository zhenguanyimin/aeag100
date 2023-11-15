#ifndef _DRONESNIFFER_H__
#define _DRONESNIFFER_H__

#include "utils.h"
#include "preProcess.h"

#define MaxDrone 20
#define MaxPulseInGroup 120
#define MaxObjNum 20
#define MaxHistoryNum 1000
#define MaxPulseTimeNum 16


struct blockGroup
{
    int   id;
    int   index;
    float range;
    char  inCh[NCh];
    float freq[MaxPulseInGroup];
    float bw;
    float burstTime[MaxPulseInGroup];
    float width[MaxPulseInGroup];
    float amp[NCh][MaxPulseInGroup];
    float dist[MaxPulseInGroup];
    int   n;
};



struct droneResult
{
    int ID;             //
    char name[50];      //
    char flag;          //
    float freq[50];     //
    float burstTime[50];//
    float width[50];    //
    float bw;           //
    float amp;          //
    float range;        //
};

void droneSniffer(struct droneResult *droneInfo, int *nDrone, float ***specMat, float cenFreq, float fs, float bw, float gain);
void printResult(struct droneResult *droneInfo, int nDrone);

#endif

