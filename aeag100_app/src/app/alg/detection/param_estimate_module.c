/* Standard Include Files. */
#include "param_estimate_module.h"
#include "detection_interface.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xil_printf.h>

#include "../../../inc/common_define.h"
#include "../../../srv/log/log.h"

/* parameter estimation processing */
unsigned int paramEstiProcessing(float (*stftOut)[SIZE_FFT], detectedPoint *detPoint, signalParameter *sigParam)
{
    unsigned int numDetPoint = 0;
    unsigned int numDetSig = 0;
    unsigned int n, j;
    unsigned int sigId;
    unsigned int pIdx, sIdx;
    int timeDiff, freqDiff;
    int jj = 0;
    unsigned int currSigID = 1;
    unsigned int maxTime = 0;
    unsigned int minTime = NUM_FFT_PER_FRAME;
    int maxFreq = 0;
    int minFreq = SIZE_FFT;
    float sumMag = 0.0;
    unsigned int numDetPointPerSig = 0;
    float acqTime;
    float freq;
    float bandwidth;
    float duration;
    float averageMag = 0.0;
    // float           averageMag1;
    sPL_SignalHandlingCfg *sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();

    /* Obtain detected signal points */
    for (n = 0; n < (NUM_FFT_PER_FRAME - 1); n++)
    {
        for (j = 0; j < (SIZE_FFT - 1); j++)
        {
            if (stftOut[n][j] > 0)
            {
                if (((stftOut[n + 1][j] - stftOut[n][j]) < g_paramEstThresholdMag) || ((stftOut[n][j + 1] - stftOut[n][j]) < g_paramEstThresholdMag))
                {
                    detPoint[numDetPoint].tidx = n;
                    detPoint[numDetPoint].fidx = j;
                    detPoint[numDetPoint].mag = stftOut[n][j];

                    numDetPoint++;
                }
            }
        }
        if (GetAlgorithmStopFlag())
        {
            return 0;
        }
    }

    /* Perform signal parameters estimation */
    if (numDetPoint > 0)
    {
        numDetSig = 1;

        /* Obtain the number of detected signals */
        for (pIdx = 0; pIdx < numDetPoint - 1; pIdx++)
        {
            timeDiff = detPoint[pIdx + 1].tidx - detPoint[pIdx].tidx;
            freqDiff = detPoint[pIdx + 1].fidx - detPoint[pIdx].fidx;

            if ((abs(timeDiff) < g_paramEstThresholdTime) && (abs(freqDiff) < g_paramEstThresholdFreq))
            {
                detPoint[pIdx].id = numDetSig;
            }
            else
            {
                detPoint[pIdx].id = numDetSig;
                numDetSig++;
            }
        }

        /* Calcualte */
        for (sIdx = 0; sIdx < numDetSig; sIdx++)
        {
            /* Estimate parameters */
            while (detPoint[jj].id == currSigID)
            {
                if (detPoint[jj].tidx > maxTime)
                {
                    maxTime = detPoint[jj].tidx;
                }

                if (detPoint[jj].tidx < minTime)
                {
                    minTime = detPoint[jj].tidx;
                }

                if (detPoint[jj].fidx > maxFreq)
                {
                    maxFreq = detPoint[jj].fidx;
                }

                if (detPoint[jj].fidx < minFreq)
                {
                    minFreq = detPoint[jj].fidx;
                }

                // sumMag = detPoint[jj].mag + sumMag;
                averageMag = averageMag * ((float)numDetPointPerSig / ((float)numDetPointPerSig + 1)) + detPoint[jj].mag / ((float)numDetPointPerSig + 1);

                numDetPointPerSig++;
                jj++;
                if (GetAlgorithmStopFlag())
                {
                    return 0;
                }
            }

            duration = ((float)(maxTime - minTime)) * TIME_RESOLUTION_US;
            bandwidth = ((float)(maxFreq - minFreq)) * FREQ_RESOLUTION_MHZ;
            acqTime = (float)minTime * TIME_RESOLUTION_US; // TBD
            freq = (sPL_Cfg->uLocalOscFreq - 20) + minFreq * FREQ_RESOLUTION_MHZ + 0.5 * bandwidth;
            // averageMag1 = sumMag / (float)numDetPointPerSig;

            /* Output results */
            sigParam[sIdx].id = currSigID;
            sigParam[sIdx].acqTime = acqTime;
            sigParam[sIdx].freq = freq;
            sigParam[sIdx].BW = bandwidth;
            sigParam[sIdx].dur = duration;
            sigParam[sIdx].amp = averageMag;

            if (g_alg_debug_printf_en == PRINT_PARA_ESTIMATE_RESULT)
            {
                LOG_DEBUG("\n********** 1 paramEstimateModule **********\r\n");
                LOG_DEBUG("id:%d	BW:%d Khz,	dur:%d us,	amp:%d,	f:%d Khz,	acqTime:%d us\r\n",
                          sIdx, (uint32_t)(bandwidth * 1000), (uint32_t)(duration), (uint32_t)(averageMag * 1000), (uint32_t)(freq * 1000), (uint32_t)(acqTime));
            }
            if (GetAlgorithmStopFlag())
            {
                return 0;
            }

            /* Update the conditions in the while loop */
            currSigID = detPoint[jj].id;
            maxTime = 0;
            minTime = NUM_FFT_PER_FRAME;
            maxFreq = 0;
            minFreq = SIZE_FFT;
            // sumMag = 0;
            numDetPointPerSig = 0;
            averageMag = 0.0;
        }
    }

    return numDetSig;
}

unsigned int paramEstiProcessing_V1_1(float (*stftOut)[SIZE_FFT], detectedPoint *detPoint, signalParameter *sigParam, unsigned int *pNumDetPoint)
{
    unsigned int numDetPoint = 0;
    unsigned int numDetSig = 0;
    unsigned int n, j;
    unsigned int sigId;
    unsigned int pIdx, sIdx;
    int timeDiff, freqDiff;
    int jj = 0;
    unsigned int currSigID = 1;
    unsigned int maxTime = 0;
    unsigned int minTime = NUM_FFT_PER_FRAME;
    int maxFreq = 0;
    int minFreq = SIZE_FFT;
    float sumMag = 0.0;
    unsigned int numDetPointPerSig = 0;
    float acqTime;
    float freq;
    float bandwidth;
    float duration;
    float averageMag = 0.0;
    // float           averageMag1;
    sPL_SignalHandlingCfg *sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();

    /* Obtain detected signal points */
    for (n = 0; n < (NUM_FFT_PER_FRAME - 1); n++)
    {
        for (j = 0; j < (SIZE_FFT - 1); j++)
        {
            if (stftOut[n][j] > 0)
            {
                if (((stftOut[n + 1][j] - stftOut[n][j]) < g_paramEstThresholdMag) || ((stftOut[n][j + 1] - stftOut[n][j]) < g_paramEstThresholdMag))
                {
                    detPoint[numDetPoint].tidx = n;
                    detPoint[numDetPoint].fidx = j;
                    detPoint[numDetPoint].mag = stftOut[n][j];

                    numDetPoint++;
                }
            }
        }
        if (GetAlgorithmStopFlag())
        {
            return 0;
        }
    }

    /* Perform signal parameters estimation */
    if (numDetPoint > 0)
    {
        numDetSig = 1;

        /* Obtain the number of detected signals */
        for (pIdx = 0; pIdx < numDetPoint - 1; pIdx++)
        {
            timeDiff = detPoint[pIdx + 1].tidx - detPoint[pIdx].tidx;
            freqDiff = detPoint[pIdx + 1].fidx - detPoint[pIdx].fidx;

            if ((abs(timeDiff) < g_paramEstThresholdTime) && (abs(freqDiff) < g_paramEstThresholdFreq))
            {
                detPoint[pIdx].id = numDetSig;
            }
            else
            {
                detPoint[pIdx].id = numDetSig;
                numDetSig++;
                if (numDetSig == MAX_NUM_SIGNAL)
                {
                    break;
                }
            }
        }

        /* Calcualte */
        for (sIdx = 0; sIdx < numDetSig; sIdx++)
        {
            /* Estimate parameters */
            while (detPoint[jj].id == currSigID)
            {
                if (detPoint[jj].tidx > maxTime)
                {
                    maxTime = detPoint[jj].tidx;
                }

                if (detPoint[jj].tidx < minTime)
                {
                    minTime = detPoint[jj].tidx;
                }

                if (detPoint[jj].fidx > maxFreq)
                {
                    maxFreq = detPoint[jj].fidx;
                }

                if (detPoint[jj].fidx < minFreq)
                {
                    minFreq = detPoint[jj].fidx;
                }

                // sumMag = detPoint[jj].mag + sumMag;
                averageMag = averageMag * ((float)numDetPointPerSig / ((float)numDetPointPerSig + 1)) + detPoint[jj].mag / ((float)numDetPointPerSig + 1);

                numDetPointPerSig++;
                jj++;
                if (GetAlgorithmStopFlag())
                {
                    return 0;
                }
            }

            duration = ((float)(maxTime - minTime)) * TIME_RESOLUTION_US;
            bandwidth = ((float)(maxFreq - minFreq)) * FREQ_RESOLUTION_MHZ;
            acqTime = (float)minTime * TIME_RESOLUTION_US; // TBD
            freq = (sPL_Cfg->uLocalOscFreq - 20) + minFreq * FREQ_RESOLUTION_MHZ + 0.5 * bandwidth;
            // averageMag1 = sumMag / (float)numDetPointPerSig;

            /* Output results */
            sigParam[sIdx].id = currSigID;
            sigParam[sIdx].acqTime = acqTime;
            sigParam[sIdx].freq = freq;
            sigParam[sIdx].BW = bandwidth;
            sigParam[sIdx].dur = duration;
            sigParam[sIdx].amp = averageMag;

            if (g_alg_debug_printf_en == PRINT_PARA_ESTIMATE_RESULT)
            {
                LOG_DEBUG("\n********** 1 paramEstimateModule **********\r\n");
                LOG_DEBUG("id:%d	BW:%d Khz,	dur:%d us,	amp:%d,	f:%d Khz,	acqTime:%d us\r\n",
                          sIdx, (uint32_t)(bandwidth * 1000), (uint32_t)(duration), (uint32_t)(averageMag * 1000), (uint32_t)(freq * 1000), (uint32_t)(acqTime));
            }
            if (GetAlgorithmStopFlag())
            {
                return 0;
            }

            /* Update the conditions in the while loop */
            currSigID = detPoint[jj].id;
            maxTime = 0;
            minTime = NUM_FFT_PER_FRAME;
            maxFreq = 0;
            minFreq = SIZE_FFT;
            // sumMag = 0;
            numDetPointPerSig = 0;
            averageMag = 0.0;
        }
    }

    *pNumDetPoint = numDetPoint;
    return numDetSig;
}
