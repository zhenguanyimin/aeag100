#include <math.h>
#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>
#include <string.h>
#include "preProcess.h"
#include "utils.h"
#include "droneSniffer.h"
#include "feature.h"
#include "downLoadSniffer.h"
#include "upLoadSniffer.h"


//特征库，包含图传和遥控
static struct DroneLib droneLIB[30]=
{
	//download
	/*
	{"DJI Phantom",0,{2406.5,2416.5,2426.5,2436.5,2446.5,2456.5,2466.5,2476.5,5735,5740,5745,5750,5755,5760,5765,5770,5775,5780,5785,5790,5795,5800,5805,5810,5815,5820,5825,5830,5835,5840},30,1,0,{9.8},1,1,{14},1,{10},1,{0.2},{5},1,4,0,1,0,0.7},
	*/

	{"DJI Mavic2/Phantom4",0,{2400,2480},2,0,0,{2.1,3.1,4.1,5.1},4,0,{4,6},2,{10,18},2,{0.2,0.2,0.2,0.2},{1,3,0,3},0.5,8,0,1,0,0.758},
	{"DJI Mavic2/Phantom4",0,{5715,5850},2,0,0,{2.1,3.1,4.1,5.1},4,0,{4,6},2,{10,18},2,{0.2,0.2,0.2,0.2},{1,3,0,3},0.5,8,0,1,0,0.758},

	{"DJI Mavic3/Air2s",0,{2400,2480},2,0,0,{1.1,2.1},2,0,{2,4,4},3,{10,18},2,{0.2,0.2},{14,4},0.2,12,0,1,0,0.41},
	{"DJI Mavic3/Air2s",0,{5715,5850},2,0,0,{1.1,2.1},2,0,{2,4,4},3,{10,18},2,{0.2,0.2},{14,4},0.2,12,0,1,0,0.41},

	/*
	{"Autel EVO2",0,{2400,2483.5},2,0,0,{10.27},1,1,{14},1,{10},1,{0.3},{5},1,4,0,1,0,0.7},
	{"Autel EVO2",0,{5725,5850},2,0,0,{10.27},1,1,{14},1,{10},1,{0.3},{5},1,4,0,1,0,0.7},
	{"Autel EVO2",0,{5650,5755},2,0,0,{10.27},1,1,{14},1,{10},1,{0.3},{5},1,4,0,1,0,0.7},

	{"Autel EVO V3",0,{2400,2483.5},2,0,0,{2.9},1,1,{5},1,{9.2},1,{0.2},{12},1,8,0,1,0,0.5},
	{"Autel EVO V3",0,{5725,5850},2,0,0,{2.9},1,1,{5},1,{9.2},1,{0.2},{12},1,8,0,1,0,0.5},
	{"Autel EVO V3",0,{5650,5755},2,0,0,{2.9},1,1,{5},1,{9.2},1,{0.2},{12},1,8,0,1,0,0.5},
	*/

	/*
	{"DJI Air2s",0,{2400,2480},2,0,0,{1.1,2.1},2,0,{2},1,{10,18},2,{0.1},{20},0.2,10,0,1,5,0.39},
	{"DJI Air2s",0,{5710,5850},2,0,0,{1.1,2.1},2,0,{2},1,{10,18},2,{0.1},{20},0.2,10,0,1,5,0.39},
	*/

	
	{"DJI Mini2",0,{2400,2480},2,0,0,{1.1,2.1},2,0,{4,6},2,{10,18},2,{0.2,0.2},{15,-3},0.3,5,0,1,5,0.223},
	{"DJI Mini2",0,{5710,5850},2,0,0,{1.1,2.1},2,0,{4,6},2,{10,18},2,{0.2,0.2},{15,-3},0.3,5,0,1,5,0.223},
	
	/*
	{"DJI Mini3pro",0,{2400,2480},2,0,0,{1.1,2.1},2,0,{10},1,{10,18},2,{0.2,0.2},{8,-3},0.3,5,0,1,5,0.11},
	{"DJI Mini3pro",0,{5710,5850},2,0,0,{1.1,2.1},2,0,{10},1,{10,18},2,{0.2,0.2},{8,-3},0.3,5,0,1,5,0.11},
	*/

	//upload
	/*
	{"Mavic 2/3",1,{2411.6,2413.6,2415.6,2417.6,2419.6,2421.6,2423.6,2425.6,2427.6,2429.6,2431.6,2433.6,2435.6,2437.6,2439.6,2441.6,2443.6,2445.6,2447.6,2449.6,2451.6,2453.6,2455.6,2457.6,2459.6,2461.6,2463.6,2465.6,2467.6,2469.6,2471.6,2473.6,2475.6,2477.6,2479.6,2481.6},36,1,2,{0.52},1,1,{4,6},2,{1.4},1,{0.06},{6},0.2,3,0.5,2,0,1},
	{"Mavic 2/3",1,{5728.6,5730.6,5732.6,5734.6,5736.6,5738.6,5740.6,5742.6,5744.6,5746.6,5748.6,5750.6,5752.6,5754.6,5756.6,5758.6,5760.6,5762.6,5764.6,5766.6,5768.6,5770.6,5772.6,5774.6,5776.6,5778.6,5780.6,5782.6,5784.6,5786.6,5788.6,5790.6,5792.6,5794.6,5796.6,5798.6,5800.6,5802.6,5804.6,5806.6,5808.6,5810.6,5812.6,5814.6,5816.6,5818.6,5820.6,5822.6,5824.6,5826.6,5828.6,5830.6,5832.6,5834.6,5836.6,5838.6,5840.6,5842.6,5844.6,5846.6},60,1,2,{0.52},1,1,{4,6},2,{1.4},1,{0.06},{6},0.2,3,0.5,2,0,1},
	*/

	/*
	{"Phantom 4A",1,{2468,2462,2456,2450,2444,2438,2432,2426,2420,2414,2408,2470,2464,2458,2452,2446,2440,2434,2428,2422,2416,2410,2404,2466,2460,2454,2448,2442,2436,2430,2424,2418,2412,2406},34,1,0,{2.2,3.2},2,1,{14},1,{1.2},1,{0.3,0.2},{2,2},1,4,0.8,2,0,1},
	{"Phantom 4A",1,{2406,2412,2418,2424,2430,2436,2442,2448,2454,2460,2466,2404,2410,2416,2422,2428,2434,2440,2446,2452,2458,2464,2470,2408,2414,2420,2426,2432,2438,2444,2450,2456,2462,2468},34,1,0,{2.2,3.2},2,1,{14},1,{1.2},1,{0.3,0.2},{2,2},1,4,0.8,2,0,1},
	{"Phantom 4 Pro",1,{2455,2431,2407,2460,2436,2412,2465,2441,2417,2470,2447,2422,2476,2452,2428,2404,2457,2433,2409,2462,2438,2414,2467,2443,2419,2472,2448,2424,2477,2453,2429,2405,2458,2435,2411,2464,2439,2415,2468,2444},40,1,0,{2.2},1,1,{14},1,{1.2},1,{0.3},{2},1,4,0.8,2,0,1},
	{"Phantom 4A",1,{2410,2420,2430,2440,2450,2460,2470,2412,2422,2432,2442,2452,2462,2404,2414,2424,2434,2444,2454,2464,2406,2416,2426,2436,2446,2456,2466,2408,2418,2428,2438,2448,2458,2468},34,1,0,{2.2,3.2},2,1,{14},1,{1.2},1,{0.3,0.2},{2,2},1,4,0.8,2,0,1},
	{"Phantom 4A",1,{2468,2458,2448,2438,2428,2418,2408,2466,2456,2446,2436,2426,2416,2406,2464,2454,2444,2434,2424,2414,2404,2462,2452,2442,2432,2422,2412,2470,2460,2450,2440,2430,2420,2410},34,1,0,{2.2,3.2},2,1,{14},1,{1.2},1,{0.3,0.2},{2,2},1,4,0.8,2,0,1},
	*/
};

static int nLib = 6;//特征库内图传和遥控种类

//对外接口
void setResult(struct droneResult *droneInfo, int *nDrone, struct DroneLib *droneLIB, struct blockGroup *uavPulse, int nUavPulse, struct blockGroup *uavCtrPulse, int nUavCtrPulse)
{
    *nDrone=nUavPulse+nUavCtrPulse;
    float maxAmp=0.0f;
    for(int i=0;i<nUavPulse;i++)
    {
        droneInfo[i].ID=uavPulse[i].id;
        strcpy(droneInfo[i].name,droneLIB[uavPulse[i].index].name);
        droneInfo[i].flag=0;
        droneInfo[i].amp=0.0f;
        for(int j=0;j<uavPulse[i].n;j++)
        {
            droneInfo[i].freq[j]=uavPulse[i].freq[j];
            droneInfo[i].burstTime[j]=uavPulse[i].burstTime[j];
            droneInfo[i].width[j]=uavPulse[i].width[j];
            maxAmp=uavPulse[i].amp[0][j];
            for(int k=0;k<NCh;k++)
            {
                if(uavPulse[i].amp[k][j]>maxAmp)
                    maxAmp=uavPulse[i].amp[k][j];
            }
            droneInfo[i].amp+=maxAmp;
        }
        droneInfo[i].bw=uavPulse[i].bw;
        droneInfo[i].amp=droneInfo[i].amp/(float)uavPulse[i].n;
        droneInfo[i].range=uavPulse[i].range;
    }

    for(int i=0;i<nUavCtrPulse;i++)
    {
        droneInfo[nUavPulse+i].ID=uavCtrPulse[i].id;
        strcpy(droneInfo[nUavPulse+i].name,droneLIB[uavCtrPulse[i].index].name);
        droneInfo[nUavPulse+i].flag=1;
        droneInfo[nUavPulse+i].amp=0.0f;
        for(int j=0;j<=uavCtrPulse[i].n;j++)
        {
            droneInfo[nUavPulse+i].freq[j]=uavCtrPulse[i].freq[j];
            droneInfo[nUavPulse+i].burstTime[j]=uavCtrPulse[i].burstTime[j];
            droneInfo[nUavPulse+i].width[j]=uavCtrPulse[i].width[j];
            maxAmp=uavCtrPulse[i].amp[0][j];
            for(int k=0;k<NCh;k++)
            {
                if(uavCtrPulse[i].amp[k][j]>maxAmp)
                    maxAmp=uavCtrPulse[i].amp[k][j];
            }
            droneInfo[nUavPulse+i].amp+=maxAmp;
        }
        droneInfo[nUavPulse+i].bw=uavCtrPulse[i].bw;
        droneInfo[nUavPulse+i].amp=droneInfo[nUavPulse+i].amp/(float)uavCtrPulse[i].n;
        droneInfo[nUavPulse+i].range=uavCtrPulse[i].range;
    }
}

//打印参数
void printResult(struct droneResult *droneInfo, int nDrone)
{
    for(int i=0;i<nDrone;i++)
    {
        printf("---------------------------------\n");
        if(droneInfo[i].flag==0)
        {
            //continue;
            printf("drone:\n");
        }
        else
        {
            //continue;
            printf("remote:\n");
        }

        printf("ID=%d\n",droneInfo[i].ID);
        printf("name=%s\n",droneInfo[i].name);

        printf("freq=%.1f,bandwidth=%.1f,amp=%.1f,range=%.1f\n",droneInfo[i].freq[0],droneInfo[i].bw,droneInfo[i].amp,droneInfo[i].range);
    }
}

/*
 * 主函数
 * specMat指向二维数组的指针，每行128个数据，共2343行;
 * cenFreq,中心频率，MHz;
 * fs,采样率，M;
 * gain,增益，dB;
 */
void droneSniffer(struct droneResult *droneInfo, int *nDrone, float ***specMat, float cenFreq, float fs, float bw, float gain)
{
    int nRows=NRow/M;
    int nCols=NFFT/N;
    float dF=fs/NFFT*N;
	int fInvNhalf = 0;
    float dt=M*NFFT/fs/1000.0f;
    
    struct blockGroup *uavPulse = (struct blockGroup *)malloc(MaxDrone * sizeof(struct blockGroup));
    int                nUavPulse = 0;
    struct blockGroup *uavCtrPulse = (struct blockGroup *)malloc(MaxDrone * sizeof(struct blockGroup));
    int                nUavCtrPulse = 0;
    
    int vidSpan[2]={0,0};
    int ctrSpan[2]={0,0};
    
    //
	fInvNhalf = (int)((fs-bw)/fs*(float)(NFFT/N/2));
    getFreqSpan(droneLIB, nLib, cenFreq, dF, fInvNhalf, vidSpan, ctrSpan);
    

    downLoadSniffer( uavPulse, &nUavPulse, specMat, nRows, nCols, cenFreq, gain, dt, dF, droneLIB, nLib, vidSpan);
    //printDroneBlock(uavPulse, nUavPulse, droneLIB);


    upLoadSniffer( uavCtrPulse, &nUavCtrPulse, specMat, nRows, nCols, cenFreq, gain, dt, dF, droneLIB, nLib, ctrSpan);
    //printRemoteBlock(uavCtrPulse, nUavCtrPulse, droneLIB);
    
    //
    setResult(droneInfo, nDrone, droneLIB, uavPulse, nUavPulse, uavCtrPulse, nUavCtrPulse);
    
    free(uavPulse);
    free(uavCtrPulse);
}


