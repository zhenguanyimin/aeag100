#include "feature.h"
#include "preProcess.h"

//获取特征库内机型在当前频段占据的频段范围。主要用于加速计算，不在范围内的频段不参与计算
void getFreqSpan(struct DroneLib *UAVtypes, int nUav, float cenFreq, float dF, int fInvNhalf, int *vidSpan, int *ctrSpan)
{
    int   j, k;
    int   minIndex[2], maxIndex[2];
    int   tp, index;


    minIndex[0] = NFFT/N-1-fInvNhalf;
    minIndex[1] = NFFT/N-1-fInvNhalf;
    maxIndex[0] = fInvNhalf;
    maxIndex[1] = fInvNhalf;
    for (j = 0; j < nUav; j++)
    {
        if (UAVtypes[j].method == 1)
            tp = 0;
        else
            tp = 1;

        if (UAVtypes[j].isFixedFreq)
        {
            for (k = 0; k < UAVtypes[j].nfreq; k++)
            {
                index = (UAVtypes[j].freqPoints[k] - cenFreq - UAVtypes[j].pulseBW[UAVtypes[j].nPulseBW - 1] / 2.0) / dF + NFFT / N / 2;
                if (index >= fInvNhalf && index < NFFT/N - fInvNhalf)
                {
                    if (index < minIndex[tp])
                        minIndex[tp] = index;
                }
                else if(index<fInvNhalf)
                {
                    minIndex[tp] = fInvNhalf;
                }
                
                index = (UAVtypes[j].freqPoints[k] - cenFreq + UAVtypes[j].pulseBW[UAVtypes[j].nPulseBW - 1] / 2.0) / dF + NFFT / N / 2;
				if (index >= fInvNhalf && index < NFFT / N - fInvNhalf)
                {
                    if (index > maxIndex[tp])
                        maxIndex[tp] = index;
                }
                else if(index>NFFT/N-1 - fInvNhalf)
                {
                    maxIndex[tp] = NFFT/N-1 - fInvNhalf;
                }
            }
        }
        else
        {
            index = (UAVtypes[j].freqPoints[0] - cenFreq) / dF + NFFT / N / 2;
            if (index >= fInvNhalf && index < NFFT / N - fInvNhalf)
            {
                if (index < minIndex[tp])
                    minIndex[tp] = index;
            }
            else if(index<fInvNhalf)
            {
                minIndex[tp]= fInvNhalf;
            }
            
            index = (UAVtypes[j].freqPoints[1] - cenFreq) / dF + NFFT / N / 2 + 1;
			if (index >= fInvNhalf && index < NFFT / N - fInvNhalf)
            {
                if (index > maxIndex[tp])
                    maxIndex[tp] = index;
            }
            else if(index>NFFT/N-1 - fInvNhalf)
            {
                maxIndex[tp]=NFFT/N-1 - fInvNhalf;
            }
        }
    }
    
    //printf("minIndex=%d,%d\n",minIndex[0],minIndex[1]);
    //printf("maxIndex=%d,%d\n",maxIndex[0],maxIndex[1]);

    if (minIndex[0] > maxIndex[0])
    {
        vidSpan[0] = fInvNhalf;
        vidSpan[1] = fInvNhalf;
    }
    else
    {
        vidSpan[0] = minIndex[0];
        vidSpan[1] = maxIndex[0];
    }

	if (minIndex[1] > maxIndex[1])
    {
        ctrSpan[0] = fInvNhalf;
        ctrSpan[1] = fInvNhalf;
    }
    else
    {
        ctrSpan[0] = minIndex[1];
        ctrSpan[1] = maxIndex[1];
    }
}


/*
 * 打印特征库参数
 */
void printUavlib(struct DroneLib *UAVtypes, int nUAV)
{
    int i = 0, j = 0;
    for (i = 0; i < nUAV; i++)
    {
        printf("id=%d\n", i);
        printf("name=%s\n", UAVtypes[i].name);
        printf("downOrUp=%d\n", UAVtypes[i].downOrUp);
        printf("freqPoints=");
        for (j = 0; j < UAVtypes[i].nfreq; j++)
            printf("%.1f,", UAVtypes[i].freqPoints[j]);
        printf("\n");
        printf("nfreqp=%d\n", UAVtypes[i].nfreq);
        printf("isFixedFreq=%d\n", UAVtypes[i].isFixedFreq);
        printf("hoppType=%d\n", UAVtypes[i].hoppType);
        printf("pulseW=");
        for (j = 0; j < UAVtypes[i].nPulseW; j++)
            printf("%.2f,", UAVtypes[i].pulseW[j]);
        printf("\n");
        printf("isFixedPulseW=%d\n", UAVtypes[i].isFixedPulseW);
        printf("pulseT=");
        for (j = 0; j < UAVtypes[i].nPulseT; j++)
            printf("%.2f,", UAVtypes[i].pulseT[j]);
        printf("\n");
        printf("nPulseT=%d\n", UAVtypes[i].nPulseT);
        printf("pulseBW=");
        for (j = 0; j < UAVtypes[i].nPulseBW; j++)
            printf("%.1f,", UAVtypes[i].pulseBW[j]);
        printf("\n");
        printf("pulseWErr=");
        for (j = 0; j < UAVtypes[i].nPulseW; j++)
            printf("%.2f,", UAVtypes[i].pulseWErr[j]);
        printf("\n");
        printf("meetPulseW=");
        for (j = 0; j < UAVtypes[i].nPulseW; j++)
            printf("%d,", UAVtypes[i].meetPulseW[j]);
        printf("\n");
        printf("pulseTErr=%.1f\n", UAVtypes[i].pulseTErr);
        printf("hoppCnt=%d\n", UAVtypes[i].hoppCnt);
        printf("freqErr=%.1f\n", UAVtypes[i].freqErr);
        printf("method=%d\n", UAVtypes[i].method);
        printf("SNR=%.1f\n", UAVtypes[i].SNR);
        printf("duty=%.3f\n\n", UAVtypes[i].duty);
    }
}

