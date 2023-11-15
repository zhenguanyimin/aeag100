/* Standard Include Files. */
#include "data_process_module.h"
#include "detection_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xil_printf.h>

#include "../../../srv/log/log.h"

float DistanceEstimate(float sigAmp, float sigFreq)
{
    float PAdBm = 0.0f;
    float atten = 0.0f;
    float distance = 0.0f; // unit in m
    float distCalcExp = 0.0f;

    PAdBm = 22.218 * sigAmp - 160.25;
    atten = 20 - PAdBm; // 20221018空衰
    distCalcExp = (atten - 20 * log10f(sigFreq) - 32.4) / 20;
    distance = (powf(10, distCalcExp)) * 1000;

    return distance;
}

unsigned int preProcessing(unsigned int numDetSig, signalParameter *sigParam, pjieguo *pwjieguo)
{
    unsigned int numDetSigFinal = 0;
    int dshu;
    int idx;
    float Perror, Berror, Terror, PAerror;
    int chenggongflag;
    float PAdBm = 0.0f;
    float atten = 0.0f;
    float Distance = 0.0f; // unit in m
    float DistCalcExp = 0.0f;

    for (dshu = 0; dshu < numDetSig; dshu++)
    {
        if (dshu == 0)
        {
            pwjieguo[dshu].id = 1;
            pwjieguo[dshu].acqTime = sigParam[dshu].acqTime;
            pwjieguo[dshu].freq = sigParam[dshu].freq;
            pwjieguo[dshu].BW = sigParam[dshu].BW;
            pwjieguo[dshu].dur = sigParam[dshu].dur;
            pwjieguo[dshu].amp = sigParam[dshu].amp;
            pwjieguo[dshu].numTtl = 1;
            pwjieguo[dshu].prt = 0;

            numDetSigFinal++;
        }
        else
        {
            chenggongflag = 0;
            idx = 0;
            while (idx < numDetSigFinal)
            {
                Perror = pwjieguo[idx].freq - sigParam[dshu].freq;
                Berror = pwjieguo[idx].BW - sigParam[dshu].BW;
                Terror = pwjieguo[idx].dur - sigParam[dshu].dur;
                PAerror = pwjieguo[idx].amp - sigParam[dshu].amp;

                if ((fabsf(Perror) < PPINLV) && (fabsf(Berror) < PDAIKUAN) && (fabsf(Terror) < PSHIKUAN) && (fabsf(PAerror) < PPATH))
                {
                    pwjieguo[idx].freq = 0.5 * (pwjieguo[idx].freq + sigParam[dshu].freq);
                    pwjieguo[idx].BW = 0.5 * (pwjieguo[idx].BW + sigParam[dshu].BW);
                    pwjieguo[idx].dur = 0.5 * (pwjieguo[idx].dur + sigParam[dshu].dur);
                    pwjieguo[idx].amp = 0.5 * (pwjieguo[idx].amp + sigParam[dshu].amp);

                    if (pwjieguo[idx].numTtl == 1)
                    {
                        pwjieguo[idx].prt = sigParam[dshu].acqTime - pwjieguo[idx].acqTime;
                    }
                    else
                    {
                        pwjieguo[idx].prt = 0.5 * (pwjieguo[idx].prt + sigParam[dshu].acqTime - pwjieguo[idx].acqTime);
                    }

                    pwjieguo[idx].numTtl = pwjieguo[idx].numTtl + 1;
                    pwjieguo[idx].acqTime = sigParam[dshu].acqTime;

                    chenggongflag = 1;

                    break;
                }

                idx++;
            }

            if (chenggongflag == 0)
            {
                pwjieguo[numDetSigFinal].id = numDetSigFinal + 1;
                pwjieguo[numDetSigFinal].acqTime = sigParam[dshu].acqTime;
                pwjieguo[numDetSigFinal].freq = sigParam[dshu].freq;
                pwjieguo[numDetSigFinal].BW = sigParam[dshu].BW;
                pwjieguo[numDetSigFinal].dur = sigParam[dshu].dur;
                pwjieguo[numDetSigFinal].amp = sigParam[dshu].amp;
                pwjieguo[numDetSigFinal].numTtl = 1;
                pwjieguo[numDetSigFinal].prt = 0;

                numDetSigFinal++;
            }
        }
    }

    if (g_alg_debug_printf_en == PRINT_DATA_PROCESS_RESULT)
    {
        LOG_DEBUG("********** 2 data_process_module: preProcessing, numDetSigFinal:%d **********\r\n", numDetSigFinal);
        for (int i = 0; i < numDetSigFinal; i++)
        {
            /* PAdBm计算 */
            PAdBm = -11.42 * pwjieguo[i].amp * pwjieguo[i].amp + 66.237 * pwjieguo[i].amp - 158.97;
            atten = 20 - PAdBm; // 20221018空衰
            DistCalcExp = (atten - 20 * log10f(pwjieguo[i].freq) - 32.4) / 20;
            Distance = (powf(10, DistCalcExp)) * 1000;

            LOG_DEBUG("id:%d	BW:%d Khz,	dur:%d us,	amp:%d,	f:%d Khz,	acqTime:%d us,	numTtl:%d   Distance:%d m\r\n",
                      pwjieguo[i].id, (uint32_t)(pwjieguo[i].BW * 1000), (uint32_t)(pwjieguo[i].dur),
                      (uint32_t)(pwjieguo[i].amp), (uint32_t)(pwjieguo[i].freq * 1000),
                      (uint32_t)(pwjieguo[i].acqTime * 1000), pwjieguo[i].numTtl,
                      (uint32_t)(Distance));
        }
    }

    return numDetSigFinal;
}

int identifyProcessing(unsigned int numDetSigFinal, pjieguo *pwjieguo)
{
    int Sucessflag = 0;
    int shangbaoflag = 0;
    int Sucesscounter = 0;
    float FKf[FKF_CNT] = {2403.5, 2405.50, 2407.5, 2408.50, 2409.50, 2411.50, 2413.5, 2414.50, 2415.5, 2417.50, 2419.5, 2421.50, 2420.50, 2423.50, 2426.50, 2427.5,
                          2429.50, 2431.50, 2432.50, 2433.5, 2435.50, 2437.5, 2438.50, 2441.50, 2443.5, 2444.50, 2445.5, 2447.50, 2449.5, 2450.50, 2451.50, 2453.50, 2455.5, 2456.50,
                          2457.5, 2459.50, 2461.50, 2462.50, 2463.50, 2465.50, 2467.5, 2468.50, 2469.5, 2471.50, 2473.5, 2475.50, 2477.50, 5726.50, 5728.50, 5730.35156250000,
                          5732.39453125000, 5734.39453125000, 5736.50, 5738.34765625000, 5739.50, 5740.32421875000, 5742.50, 5744.36718750000, 5745.50, 5746.34375000000,
                          5748.50, 5750.29687500000, 5751.50, 5752.36328125000, 5754.50, 5756.31640625000, 5757.50, 5758.29296875000, 5760.50, 5762.24609375000,
                          5763.50, 5764.31250000000, 5766.50, 5768.35546875000, 5769.50, 5770.33203125000, 5774.28515625000, 5775.50, 5776.26171875000, 5778.50,
                          5780.25976562500, 5781.50, 5782.28125000000, 5784.50, 5786.18945312500, 5787.50, 5788.30078125000, 5790.50, 5792.25390625000,
                          5793.50, 5794.23046875000, 5796.20703125000, 5798.27343750000, 5800.25000000000, 5802.39062500000, 5804.20312500000,
                          5806.17968750000, 5807.50, 5808.20117187500, 5814.50, 5817.50, 5818.38281250000, 5820.77326939408, 5822.35839843750, 5823.50,
                          5824.31250000000, 5826.50, 5828.50, 5829.50, 5830.50, 5832.50, 5834.50, 5835.50, 5836.50, 5838.50, 5840.50, 5842.50, 5866.50, 5867.50, 5868.50, 5856.5};
    float FKB[FKB_CNT] = {2.5};
    float FKT[FKT_CNT] = {500, 350};
    float TCf[TCF_CNT] = {2438, 5758, 5808};
    //    float TCB[TCB_CNT] = {9,18};
    float TCB[TCB_CNT] = {6.5, 9, 19};
    //    float TCT[7] = {600,1000,1400,2000,3000,4000,4400};
    float TCT[TCT_CNT] = {600, 1200, 1600, 2000, 2600, 3000, 4000, 4400};

    float FKFer = 0.9;
    float FKBerr = 1.5;
    float FKTerr = 50;

    float TCFer = 32;
    float TCBerr = 1.5;
    //    float TCBerr = 2.5;
    float TCTerr = 200;
    int countererr = 1;

    int SBdshu, SBFKPpshu, SBFKBpshu;
    int TCSBFKPpshu, TCSBFKBpshu, TCSBFKTpshu;
    float FKBerror, FKTerror;
    float TCFKPerror, TCFKBerror, TCFKTerror;
    float PAdBm = 0.0f;
    float atten = 0.0f;
    float Distance = 0.0f; // unit in m
    float DistCalcExp = 0.0f;

    for (SBdshu = 0; SBdshu < numDetSigFinal; SBdshu++)
    {
        int FPPflag = 0;
        float FPPshu = 0;
        int BPPflag = 0;
        float BPPshu = 0;
        int TPPflag = 0;
        float TPPshu = 0;
        int PPPflag = 0;
        float PPPshu = 0;

        int TCFPPflag = 0;
        float TCFPPshu = 0;
        int TCBPPflag = 0;
        float TCBPPshu = 0;
        int TCTPPflag = 0;
        float TCTPPshu = 0;
        int TCPPPflag = 0;
        float TCPPPshu = 0;

        //        if(((pwjieguo[SBdshu].BW <= 10 ) && (pwjieguo[SBdshu].BW >= 6 )) || ((pwjieguo[SBdshu].BW <= 2) && (pwjieguo[SBdshu].BW >= 4)) || (pwjieguo[SBdshu].dur >=300))
        {
            if (pwjieguo[SBdshu].BW <= 5)
            {
                /************ 飞控处理 ***********/
                for (SBFKPpshu = 0; SBFKPpshu < FKF_CNT; SBFKPpshu++)
                {
                    float FKPerror = pwjieguo[SBdshu].freq - FKf[SBFKPpshu]; //信号频率

                    if ((FKPerror > -FKFer) && (FKPerror < FKFer))
                    {
                        FPPflag = 1;
                    }
                    else
                    {
                        FPPflag = FPPflag;
                    }
                }

                for (SBFKBpshu = 0; SBFKBpshu < FKB_CNT; SBFKBpshu++)
                {
                    FKBerror = pwjieguo[SBdshu].BW - FKB[SBFKBpshu]; //信号带宽

                    if ((FKBerror > -FKBerr) && (FKBerror < FKBerr))
                    {
                        BPPflag = 1;
                    }
                    else
                    {
                        BPPflag = BPPflag;
                    }
                }

                for (int SBFKTpshu = 0; SBFKTpshu < FKT_CNT; SBFKTpshu++)
                {
                    FKTerror = pwjieguo[SBdshu].dur - FKT[SBFKTpshu]; //信号时宽
                    if ((FKTerror > -FKTerr) && (FKTerror < FKTerr))
                    {
                        TPPflag = 1;
                    }
                    else
                    {
                        TPPflag = TPPflag;
                    }
                }

                if ((FPPflag == 1) && (BPPflag == 1) && (TPPflag == 1))
                {
                    Sucessflag = 1;
                    Sucesscounter = Sucesscounter + 1;
                }

                //                else
                //                {
                //                    Sucessflag=Sucessflag;
                //                }
            }
            /************ 图传处理***********/
            else
            {
                for (TCSBFKPpshu = 0; TCSBFKPpshu < TCF_CNT; TCSBFKPpshu++)
                {
                    TCFKPerror = pwjieguo[SBdshu].freq - TCf[TCSBFKPpshu]; //信号频率
                    if ((TCFKPerror > -TCFer) && (TCFKPerror < TCFer))
                    {
                        TCFPPflag = 1;
                    }
                    else
                    {
                        TCFPPflag = TCFPPflag;
                    }
                }

                for (TCSBFKBpshu = 0; TCSBFKBpshu < TCB_CNT; TCSBFKBpshu++)
                {
                    TCFKBerror = pwjieguo[SBdshu].BW - TCB[TCSBFKBpshu]; //信号带宽
                    if ((TCFKBerror > -TCBerr) && (TCFKBerror < TCBerr))
                    {
                        TCBPPflag = 1;
                    }
                    else
                    {
                        TCBPPflag = TCBPPflag;
                    }
                }

                for (TCSBFKTpshu = 0; TCSBFKTpshu < TCT_CNT; TCSBFKTpshu++)
                {
                    TCFKTerror = pwjieguo[SBdshu].dur - TCT[TCSBFKTpshu]; //信号时宽
                    if ((TCFKTerror > -TCTerr) && (TCFKTerror < TCTerr))
                    {
                        TCTPPflag = 1;
                    }
                    else
                    {
                        TCTPPflag = TCTPPflag;
                    }
                }

                /* PAdBm计算 */
                PAdBm = -11.42 * pwjieguo[SBdshu].amp * pwjieguo[SBdshu].amp + 66.237 * pwjieguo[SBdshu].amp - 158.97;
                atten = 20 - PAdBm; // 20221018空衰

                if ((TCFPPflag == 1) && (TCBPPflag == 1) && (TCTPPflag == 1))
                {
                    Sucessflag = 1;
                    Sucesscounter = Sucesscounter + 1;
                    //                    Distance=10^((atten-20*log10(2421)-32.4)/20)
                    DistCalcExp = (atten - 20 * log10f(pwjieguo[SBdshu].freq) - 32.4) / 20;
                    Distance = (powf(10, DistCalcExp)) * 1000;
                }
                //                else
                //                {
                //                    Sucessflag=Sucessflag;
                //                }
            }
        }
        //        else
        //        {
        //            Sucessflag=Sucessflag;
        //        }
    }

    if ((Sucessflag == 1) && (Sucesscounter >= countererr))
    {
        shangbaoflag = 1;
    }
    else
    {
        shangbaoflag = 0;
    }

    if (g_alg_debug_printf_en == PRINT_DATA_PROCESS_RESULT)
    {
        LOG_DEBUG("********** 3 data_process_module: identifyProcessing **********\r\n");
        LOG_DEBUG("shangbaoflag %d	Sucessflag:%d	Sucesscounter:%d	PAdBm = %d mdBm	  Distance:%d 	m\r\n",
                  shangbaoflag, Sucessflag, Sucesscounter, (int32_t)(PAdBm * 1000), (uint32_t)(Distance * 1000));
    }

    return shangbaoflag;
}

unsigned int preProcessing_V1_1(unsigned int numDetSig, signalParameter *sigParam, pjieguoInfo *pwjieguoInfo)
{
    int i = 0;
    unsigned int numDetSigFinal = 0;
    int dshu;
    int idx;
    float Perror, Berror, Terror, PAerror;
    int chenggongflag;

    for (dshu = 0; dshu < numDetSig; dshu++)
    {
        if (sigParam[dshu].BW < PRE_PROC_BW_TH)
            continue;
        if (sigParam[dshu].dur < PRE_PROC_DUR_TH)
            continue;

        if (dshu == 0)
        {
            pwjieguoInfo->signalList[dshu].id = 1;
            pwjieguoInfo->signalList[dshu].acqTime = sigParam[dshu].acqTime;
            pwjieguoInfo->signalList[dshu].freq = sigParam[dshu].freq;
            pwjieguoInfo->signalList[dshu].BW = sigParam[dshu].BW;
            pwjieguoInfo->signalList[dshu].dur = sigParam[dshu].dur;
            pwjieguoInfo->signalList[dshu].amp = sigParam[dshu].amp;
            pwjieguoInfo->signalList[dshu].numTtl = 1;
            pwjieguoInfo->signalList[dshu].prt = 0;
            pwjieguoInfo->signalList[dshu].distance = DistanceEstimate(pwjieguoInfo->signalList[dshu].amp, pwjieguoInfo->signalList[dshu].freq);

            numDetSigFinal++;
        }
        else
        {
            chenggongflag = 0;
            idx = 0;
            while (idx < numDetSigFinal)
            {
                Perror = pwjieguoInfo->signalList[idx].freq - sigParam[dshu].freq;
                Berror = pwjieguoInfo->signalList[idx].BW - sigParam[dshu].BW;
                Terror = pwjieguoInfo->signalList[idx].dur - sigParam[dshu].dur;
                PAerror = pwjieguoInfo->signalList[idx].amp - sigParam[dshu].amp;

                if ((fabsf(Perror) < PPINLV) && (fabsf(Berror) < PDAIKUAN) && (fabsf(Terror) < PSHIKUAN) && (fabsf(PAerror) < PPATH))
                {
                    pwjieguoInfo->signalList[idx].freq = 0.5 * (pwjieguoInfo->signalList[idx].freq + sigParam[dshu].freq);
                    pwjieguoInfo->signalList[idx].BW = 0.5 * (pwjieguoInfo->signalList[idx].BW + sigParam[dshu].BW);
                    pwjieguoInfo->signalList[idx].dur = 0.5 * (pwjieguoInfo->signalList[idx].dur + sigParam[dshu].dur);
                    pwjieguoInfo->signalList[idx].amp = 0.5 * (pwjieguoInfo->signalList[idx].amp + sigParam[dshu].amp);

                    if (pwjieguoInfo->signalList[idx].numTtl == 1)
                    {
                        pwjieguoInfo->signalList[idx].prt = sigParam[dshu].acqTime - pwjieguoInfo->signalList[idx].acqTime;
                    }
                    else
                    {
                        pwjieguoInfo->signalList[idx].prt = 0.5 * (pwjieguoInfo->signalList[idx].prt + sigParam[dshu].acqTime - pwjieguoInfo->signalList[idx].acqTime);
                    }

                    pwjieguoInfo->signalList[idx].numTtl = pwjieguoInfo->signalList[idx].numTtl + 1;
                    pwjieguoInfo->signalList[idx].acqTime = sigParam[dshu].acqTime;
                    pwjieguoInfo->signalList[idx].distance = DistanceEstimate(pwjieguoInfo->signalList[idx].amp, pwjieguoInfo->signalList[idx].freq);

                    chenggongflag = 1;
                    break;
                }
                idx++;
            }

            if (chenggongflag == 0)
            {
                pwjieguoInfo->signalList[numDetSigFinal].id = numDetSigFinal + 1;
                pwjieguoInfo->signalList[numDetSigFinal].acqTime = sigParam[dshu].acqTime;
                pwjieguoInfo->signalList[numDetSigFinal].freq = sigParam[dshu].freq;
                pwjieguoInfo->signalList[numDetSigFinal].BW = sigParam[dshu].BW;
                pwjieguoInfo->signalList[numDetSigFinal].dur = sigParam[dshu].dur;
                pwjieguoInfo->signalList[numDetSigFinal].amp = sigParam[dshu].amp;
                pwjieguoInfo->signalList[numDetSigFinal].numTtl = 1;
                pwjieguoInfo->signalList[numDetSigFinal].prt = 0;
                pwjieguoInfo->signalList[numDetSigFinal].distance = DistanceEstimate(pwjieguoInfo->signalList[numDetSigFinal].amp, pwjieguoInfo->signalList[numDetSigFinal].freq);

                numDetSigFinal++;
                if (numDetSigFinal == MAX_NUM_SIGNAL_FINAL)
                {
                    break;
                }
            }
        }
    }

    if (g_alg_debug_printf_en == PRINT_DATA_PROCESS_RESULT)
    {
        LOG_DEBUG("********** 2 data_process_module: preProcessing, numDetSigFinal:%d **********\r\n", numDetSigFinal);
        for (i = 0; i < numDetSigFinal; i++)
        {
            LOG_DEBUG("id:%d	BW:%d Khz,	dur:%d us,	amp:%d,	f:%d Khz,	acqTime:%lld us,	numTtl:%d,	Distance:%d m\r\n",
                      pwjieguoInfo->signalList[i].id, (uint32_t)(pwjieguoInfo->signalList[i].BW * 1000), (uint32_t)(pwjieguoInfo->signalList[i].dur),
                      (uint32_t)(pwjieguoInfo->signalList[i].amp * 1000), (uint32_t)(pwjieguoInfo->signalList[i].freq * 1000),
                      (uint64_t)(pwjieguoInfo->signalList[i].acqTime), pwjieguoInfo->signalList[i].numTtl,
                      (uint32_t)(pwjieguoInfo->signalList[i].distance));
        }
    }
    pwjieguoInfo->signalNum = numDetSigFinal;

    return numDetSigFinal;
}

int identifyProcessing_V1_1(unsigned int numDetSigFinal, pjieguoInfo *pwjieguoInfo)
{
    int Sucessflag = 0;
    int shangbaoflag = 0;
    int Sucesscounter = 0;
    int signalIdxUAV = -1;
    //    float FKf[FKF_CNT] = {2403.5, 2405.50, 2407.5, 2408.50, 2409.50, 2411.50, 2413.5, 2414.50, 2415.5, 2417.50,
    //    		2419.5, 2421.50, 2420.50, 2423.50, 2426.50, 2427.5, 2429.50, 2431.50, 2432.50, 2433.5,
    //			2435.50, 2437.5, 2438.50, 2441.50, 2443.5, 2444.50, 2445.5, 2447.50, 2449.5, 2450.50,
    //			2451.50, 2453.50, 2455.5, 2456.50, 2457.5, 2459.50, 2461.50, 2462.50, 2463.50, 2465.50,
    //			2467.5, 2468.50, 2469.5, 2471.50, 2473.5, 2475.50, 2477.50, 5726.50, 5728.50, 5730.35156250000, 5746.34375000000,
    //			5748.50, 5750.29687500000, 5751.50, 5752.36328125000, 5754.50, 5756.31640625000, 5757.50, 5758.29296875000, 5760.50, 5762.24609375000,
    //			5763.50, 5764.31250000000, 5766.50, 5768.35546875000, 5769.50, 5770.33203125000, 5774.28515625000, 5775.50, 5776.26171875000, 5778.50,
    //			5780.25976562500, 5781.50, 5782.28125000000, 5784.50, 5786.18945312500, 5787.50, 5788.30078125000, 5790.50, 5792.25390625000, 5793.50, 5794.23046875000,
    //			5796.20703125000, 5798.27343750000, 5800.25000000000, 5802.39062500000, 5804.20312500000, 5806.17968750000, 5807.50, 5808.20117187500, 5814.50,
    //			5817.50, 5818.38281250000, 5820.77326939408, 5822.35839843750, 5823.50, 5824.31250000000, 5826.50, 5828.50, 5829.50, 5830.50,
    //			5832.50, 5834.50, 5835.50, 5836.50, 5838.50, 5840.50, 5842.50, 5866.50, 5867.50, 5868.50,
    //			5856.5, 836, 842, 848, 854, 860, 907, 911, 916, 921,
    //			926, 2425, 2445, 5157, 5181, 5205, 5215, 5229, 5732, 5756,
    //			5780, 5804, 5828, 2424.5, 5159.5, 5160, 5161, 2422.5, 2430.5, 5217,
    //			5218, 832, 835, 838, 840, 841, 912};
    float FKf[FKF_CNT] = {832, 835, 836, 838, 840, 841, 842, 848, 854, 860,
                          907, 911, 912, 916, 921, 926, 2403.5, 2405.50, 2407.5, 2408.50,
                          2409.50, 2411.50, 2413.5, 2414.50, 2415.5, 2417.50, 2419.5, 2421.50, 2420.50, 2422.5,
                          2423.50, 2424.5, 2425, 2426.50, 2427.5, 2429.50, 2430.5, 2431.50, 2432.50, 2433.5,
                          2435.50, 2437.5, 2438.50, 2441.50, 2443.5, 2444.50, 2445, 2445.5, 2447.50, 2449.5,
                          2450.50, 2451.50, 2453.50, 2455.5, 2456.50, 2457.5, 2459.50, 2461.50, 2462.50, 2463.50,
                          2465.50, 2467.5, 2468.50, 2469.5, 2471.50, 2473.5, 2475.50, 2477.50, 5159.5, 5160,
                          5161, 5157, 5181, 5205, 5215, 5217, 5218, 5229, 5726.50, 5728.50,
                          5730.35156250000, 5732, 5746.34375000000, 5748.50, 5750.29687500000, 5751.50, 5752.36328125000, 5754.50, 5756, 5756.31640625000,
                          5780, 5757.50, 5758.29296875000, 5760.50, 5762.24609375000, 5763.50, 5764.31250000000, 5766.50, 5768.35546875000, 5769.50,
                          5770.33203125000, 5774.28515625000, 5775.50, 5776.26171875000, 5778.50, 5780.25976562500, 5781.50, 5782.28125000000, 5784.50,
                          5786.18945312500, 5787.50, 5788.30078125000, 5790.50, 5792.25390625000, 5793.50, 5794.23046875000, 5796.20703125000, 5798.27343750000, 5800.25000000000, 5802.39062500000,
                          5804, 5804.20312500000, 5806.17968750000, 5807.50, 5808.20117187500, 5814.50, 5817.50, 5818.38281250000, 5820.77326939408, 5822.35839843750,
                          5823.50, 5824.31250000000, 5826.50, 5828, 5828.50, 5829.50, 5830.50, 5832.50, 5834.50, 5835.50,
                          5836.50, 5838.50, 5840.50, 5842.50, 5856.5, 5866.50, 5867.50, 5868.50};

    float FKB[FKB_CNT] = {3};
    float FKT[FKT_CNT] = {500, 2100, 2200, 1850, 1750, 2850};
    float TCf[TCF_CNT] = {2438, 5758, 5808};
    float TCB[TCB_CNT] = {9, 19, 11, 21};
    float TCT[TCT_CNT] = {1000, 1200, 1600, 2000, 2600, 3000, 4000, 4400, 5000};

    float FKFer = 0.9;
    float FKBerr = 1.5;
    float FKTerr = 50;

    float TCFer = 32;
    float TCBerr = 1.5;
    //    float TCBerr = 2.5;
    float TCTerr = 200;
    int countererr = 1;

    int SBdshu, SBFKPpshu, SBFKBpshu, SBFKTpshu;
    int TCSBFKPpshu, TCSBFKBpshu, TCSBFKTpshu;
    float FKPerror, FKBerror, FKTerror;
    float TCFKPerror, TCFKBerror, TCFKTerror;

    int FPPflag = 0, BPPflag = 0, TPPflag = 0, PPPflag = 0;
    float FPPshu = 0, BPPshu = 0, TPPshu = 0, PPPshu = 0;
    int TCFPPflag = 0; //频率 F
    int TCBPPflag = 0; //带宽 BW
    int TCTPPflag = 0; // 时宽 dur
    int TCPPPflag = 0;
    float TCFPPshu = 0, TCBPPshu = 0, TCTPPshu = 0, TCPPPshu = 0;

    float fDistanceMin = 1000000.0f;

    for (SBdshu = 0; SBdshu < numDetSigFinal; SBdshu++)
    {
        FPPflag = 0;
        BPPflag = 0;
        TPPflag = 0;
        PPPflag = 0;
        FPPshu = 0;
        BPPshu = 0;
        TPPshu = 0;
        PPPshu = 0;
        TCFPPflag = 0;
        TCBPPflag = 0;
        TCTPPflag = 0;
        TCPPPflag = 0;
        TCFPPshu = 0;
        TCBPPshu = 0;
        TCTPPshu = 0;
        TCPPPshu = 0;

        if (pwjieguoInfo->signalList[SBdshu].BW <= 5)
        {
            /************ 飞控处理 ***********/
            for (SBFKPpshu = 0; SBFKPpshu < FKF_CNT; SBFKPpshu++)
            {
                FKPerror = pwjieguoInfo->signalList[SBdshu].freq - FKf[SBFKPpshu]; //信号频率
                if ((FKPerror > -FKFer) && (FKPerror < FKFer))
                {
                    FPPflag = 1;
                }
                else
                {
                    FPPflag = FPPflag;
                }
            }

            for (SBFKBpshu = 0; SBFKBpshu < FKB_CNT; SBFKBpshu++)
            {
                FKBerror = pwjieguoInfo->signalList[SBdshu].BW - FKB[SBFKBpshu]; //信号带宽
                if ((FKBerror > -FKBerr) && (FKBerror < FKBerr))
                {
                    BPPflag = 1;
                }
                else
                {
                    BPPflag = BPPflag;
                }
            }

            for (SBFKTpshu = 0; SBFKTpshu < FKT_CNT; SBFKTpshu++)
            {
                FKTerror = pwjieguoInfo->signalList[SBdshu].dur - FKT[SBFKTpshu]; //信号时宽
                if ((FKTerror > -FKTerr) && (FKTerror < FKTerr))
                {
                    TPPflag = 1;
                }
                else
                {
                    TPPflag = TPPflag;
                }
            }

            if ((FPPflag == 1) && (BPPflag == 1) && (TPPflag == 1))
            {
                Sucessflag = 1;
                Sucesscounter = Sucesscounter + 1;
                if (pwjieguoInfo->signalList[SBdshu].distance < fDistanceMin)
                {
                    fDistanceMin = pwjieguoInfo->signalList[SBdshu].distance;
                    signalIdxUAV = SBdshu;
                }
            }
            else
            {
                Sucessflag = Sucessflag;
            }
        }
        /************ 图传处理***********/
        else
        {
            if (g_alg_identify_tc_match_en)
            {
                for (TCSBFKPpshu = 0; TCSBFKPpshu < TCF_CNT; TCSBFKPpshu++)
                {
                    TCFKPerror = pwjieguoInfo->signalList[SBdshu].freq - TCf[TCSBFKPpshu]; //信号频率
                    if ((TCFKPerror > -TCFer) && (TCFKPerror < TCFer))
                    {
                        TCFPPflag = 1;
                    }
                    else
                    {
                        TCFPPflag = TCFPPflag;
                    }
                }

                for (TCSBFKBpshu = 0; TCSBFKBpshu < TCB_CNT; TCSBFKBpshu++)
                {
                    TCFKBerror = pwjieguoInfo->signalList[SBdshu].BW - TCB[TCSBFKBpshu]; //信号带宽
                    if ((TCFKBerror > -TCBerr) && (TCFKBerror < TCBerr))
                    {
                        TCBPPflag = 1;
                    }
                    else
                    {
                        TCBPPflag = TCBPPflag;
                    }
                }

                for (TCSBFKTpshu = 0; TCSBFKTpshu < TCT_CNT; TCSBFKTpshu++)
                {
                    TCFKTerror = pwjieguoInfo->signalList[SBdshu].dur - TCT[TCSBFKTpshu]; //信号时宽
                    if ((TCFKTerror > -TCTerr) && (TCFKTerror < TCTerr))
                    {
                        TCTPPflag = 1;
                    }
                    else
                    {
                        TCTPPflag = TCTPPflag;
                    }
                }

                if ((TCFPPflag == 1) && (TCBPPflag == 1) && (TCTPPflag == 1))
                {
                    Sucessflag = 1;
                    Sucesscounter = Sucesscounter + 1;
                    if (pwjieguoInfo->signalList[SBdshu].distance < fDistanceMin)
                    {
                        fDistanceMin = pwjieguoInfo->signalList[SBdshu].distance;
                        signalIdxUAV = SBdshu;
                    }
                }
                else
                {
                    Sucessflag = Sucessflag;
                }
            }
        }
    }

    if ((Sucessflag == 1) && (Sucesscounter >= countererr))
    {
        shangbaoflag = 1;
        pwjieguoInfo->outputWarnDistUAV = fDistanceMin;
    }
    else
    {
        shangbaoflag = 0;
    }

    pwjieguoInfo->flagUAV = shangbaoflag;
    pwjieguoInfo->signalIdxUAV = signalIdxUAV;

    if (g_alg_debug_printf_en == PRINT_DATA_PROCESS_RESULT)
    {
        LOG_DEBUG("********** 3 data_process_module: identifyProcessing **********\r\n");
        LOG_DEBUG("shangbaoflag %d	Sucessflag:%d	Sucesscounter:%d	signalIdxUAV:%d\r\n",
                  shangbaoflag, Sucessflag, Sucesscounter, signalIdxUAV);
        if (signalIdxUAV >= 0)
        {
            LOG_DEBUG("id:%d	BW:%d Khz,	dur:%d us,	amp:%d,	f:%d Khz,	acqTime:%lld us,	numTtl:%d,	Distance:%d m\r\n",
                      pwjieguoInfo->signalList[signalIdxUAV].id,
                      (uint32_t)(pwjieguoInfo->signalList[signalIdxUAV].BW * 1000),
                      (uint32_t)(pwjieguoInfo->signalList[signalIdxUAV].dur),
                      (uint32_t)(pwjieguoInfo->signalList[signalIdxUAV].amp * 1000),
                      (uint32_t)(pwjieguoInfo->signalList[signalIdxUAV].freq * 1000),
                      (uint64_t)(pwjieguoInfo->signalList[signalIdxUAV].acqTime),
                      pwjieguoInfo->signalList[signalIdxUAV].numTtl,
                      (uint32_t)(pwjieguoInfo->signalList[signalIdxUAV].distance));
        }
    }

    return shangbaoflag;
}
