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

//获取区分信号和噪声的阈值
float autoThresh(float *s, int n, float dT, int maxIter)
{
    int   i = 0, k = 0;
    int   n1 = 0;
    float t, tnew = 0.0f;
    float t1 = 0.0f, t2 = 0.0f;
    for (i = 0; i < n; i++)
    {
        tnew += s[i] / n;
    }
    t = tnew + 2 * dT;
    while (fabs(tnew - t) > dT && k < maxIter)
    {
        t = tnew;
        for (i = 0; i < n; i++)
        {
            if (s[i] > t)
            {
                t1 += s[i];
                n1++;
            }
            else
                t2 += s[i];
        }
        if(n>n1)
            tnew = (t1 / n1 + t2 / (n - n1)) / 2.0f;
        else
            tnew=t;

        t1 = 0.0f;
        t2 = 0.0f;
        n1 = 0;
        k++;
    }
    return tnew;
}

//根据阈值划分出频谱中信号的时间起点q1和持续宽度w
int threshold(float *s, int n, float thresh, int delt, int minw, int *q1, int *w, float *colSnr)
{
	int i,j;
    char  flag = 1;
    int   nFall = 0;
    int   nw = 0;
    float signalAmp = 0.0f, noiseAmp = 0.0f;

    q1[0] = -1;
    for (i = 0; i < n; i++)
    {
        if (s[i] > thresh)
        {
            if (!flag)
            {
                q1[nw] = i;
                flag = 1;
            }
            nFall = 0;
        }
        else
        {
            if (flag)
            {
                nFall++;
                if (nFall >= delt)
                {
                    if (q1[0] > 0)
                    {
						// dingyu, 20230202
                        w[nw] = i - delt - q1[nw];
                        if (w[nw] > minw)
                        {
                            nw++;
                        }
                    }
                    flag = 0;
                }
            }
        }
    }

    if (nw > 0)
    {
        for (i = 0; i < nw; i++)
        {
            signalAmp = 0.0f;
            noiseAmp = 0.0f;
            for (j = q1[i]; j <= q1[i] + w[i]; j++)
            {
                signalAmp += s[j];
            }
            signalAmp /= w[i] + 1;
            if (i == 0)
            {
                for (j = q1[i] - 3; j < q1[i]; j++)
                {
                    noiseAmp += s[j];
                }
                noiseAmp /= 3.0f;
            }
            else
            {
                for (j = q1[i - 1] + w[i - 1] + 1; j < q1[i]; j++)
                {
                    noiseAmp += s[j];
                }
                noiseAmp /= q1[i] - q1[i - 1] - w[i - 1] - 1;
            }
            colSnr[i] = signalAmp - noiseAmp;
        }
    }

    return nw;
}

//周期匹配
void matchPulseT(int **match, float *t2, int *index, int n, float *pulseT, int nPulseT, float pulseTErr)
{
    float diffTime, resTime, TErr;
    float sumPulseT = 0.0f;
    int   nt = 0, nT = 0, nBigT = 0;
    int   count = 0, maxCount = 0, maxIndex = 0;
	int i,j,k,p,m;
	int   oneMatch[5][MaxNumVidT];
    float oneMatchTimeErr[5][MaxNumVidT];

    for (i = 0; i < nPulseT; i++)
        sumPulseT += pulseT[i];

    for (i = 0; i < n; i++)
    {
        if(i>MaxNumVidT-1)
            break;
        for (k = 0; k < nPulseT; k++)
        {
            oneMatch[k][0] = index[i];
            for (m = 1; m < MaxNumVidT; m++)
                oneMatch[k][m] = -1;

            for (j = 0; j < n; j++)
            {
                if(j>MaxNumVidT-1)
                    break;
                if (i == j)
                    continue;

                diffTime = t2[index[j]] - t2[index[i]];
                if (diffTime < 0)
                    continue;

                nBigT = diffTime / sumPulseT;
                resTime = diffTime - nBigT * sumPulseT;

                nt = 0;
                while (1)
                {
                    TErr = fabs(resTime);
                    if (TErr < pulseTErr)
                    {
                        nT = nPulseT * nBigT + nt;
                        if (nT > 0 && nT < MaxNumVidT)
                        {
                            if (oneMatch[k][nT] < 0)
                            {
                                oneMatch[k][nT] = index[j];
                                oneMatchTimeErr[k][nT] = TErr;
                            }
                            else if (TErr < oneMatchTimeErr[k][nT])
                            {
                                oneMatch[k][nT] = index[j];
                                oneMatchTimeErr[k][nT] = TErr;
                            }
                        }
                        break;
                    }
                    if (resTime < 0)
                        break;
                    resTime -= pulseT[(k + nt) % nPulseT];
                    nt++;
                }
            }
        }

        maxCount = 0;
        maxIndex = 0;
        for (k = 0; k < nPulseT; k++)
        {
            count = 0;
            for (p = 0; p < MaxNumVidT; p++)
            {
                if (oneMatch[k][p] > 0)
                {
                    count++;
                }
            }
            if (count > maxCount)
            {
                maxCount = count;
                maxIndex = k;
            }
        }

        for (k = 0; k < MaxNumVidT; k++)
        {
            match[i][k] = oneMatch[maxIndex][k];
        }
    }
}

//图传图块检测
int vidBlocksDetect(
    struct BlockRow *aBlkRow, float *s, int n, int *q1, int *w, int nw, float *colSnr, int antIndex, int colIndex, float cenFreq, float dT, float dF, struct DroneLib *UAVtypes, int nUAV)
{
    int   i, j, k;
    char  isUAV;
    char  isInFreqSpan;
    int   mPulseW[maxPulseWNumInLib];
    int * Indexq = (int *)malloc(n * sizeof(int));
    int   nIndex;
    float freqL, freqR;
    int   freqIndexL, freqIndexR;
    int   count = 0;

    int **match=(int**)malloc(MaxVidPulse*sizeof(int*));
    for(i=0;i<MaxVidPulse;i++)
        match[i]=(int*)malloc(MaxNumVidT*sizeof(int));
    
    int   meetPulseTCount = 0;
    float minSnr = 0.0;

    float *t2 = (float *)malloc(nw * sizeof(float));
    for (i = 0; i < nw; i++)
        t2[i] = (q1[i] + w[i]) * dT;

    for (i = 0; i < nUAV; i++)
    {
        if (UAVtypes[i].method != 1)
            continue;

        isUAV = 1;

        if (UAVtypes[i].isFixedFreq)
        {
            isInFreqSpan = 0;
            for (j = 0; j < UAVtypes[i].nfreq; j++)
            {
                freqL = UAVtypes[i].freqPoints[j] - UAVtypes[i].pulseBW[UAVtypes[i].nPulseBW - 1] / 2;
                freqR = UAVtypes[i].freqPoints[j] + UAVtypes[i].pulseBW[UAVtypes[i].nPulseBW - 1] / 2;
                freqIndexL = (freqL - cenFreq) / dF + NFFT / N / 2;
                freqIndexR = (freqR - cenFreq) / dF + NFFT / N / 2;
                if (colIndex > freqIndexL && colIndex <= freqIndexR)
                {
                    isInFreqSpan = 1;
                    break;
                }
            }
            if (!isInFreqSpan)
                continue;
        }
        else
        {
            freqL = UAVtypes[i].freqPoints[0];
            freqR = UAVtypes[i].freqPoints[1];
            freqIndexL = (freqL - cenFreq) / dF + NFFT / N / 2;
            freqIndexR = (freqR - cenFreq) / dF + NFFT / N / 2;

            if (colIndex <= freqIndexL || colIndex > freqIndexR)
                continue;
        }

        for (j = 0; j < maxPulseWNumInLib; j++)
            mPulseW[j] = 0;
        nIndex = 0;
        
        for (j = 0; j < nw; j++)
        {
            for (k = 0; k < UAVtypes[i].nPulseW; k++)
            {
                if (fabs(UAVtypes[i].pulseW[k] - w[j] * dT) < UAVtypes[i].pulseWErr[k])
                {
                    Indexq[nIndex] = j;
                    nIndex++;
                    mPulseW[k]++;
                    break;
                }
            }
        }
        
        for (j = 0; j < UAVtypes[i].nPulseW; j++)
        {
            if (UAVtypes[i].meetPulseW[j] > 0 && mPulseW[j] < UAVtypes[i].meetPulseW[j])
            {
                isUAV = 0;
                break;
            }
			if (UAVtypes[i].meetPulseW[j] < 0 && mPulseW[j] >= -UAVtypes[i].meetPulseW[j])
			{
				isUAV = 0;
				break;
			}
        }

        if (isUAV && UAVtypes[i].nPulseT > 0 && UAVtypes[i].pulseT[0] > 0)
        {           
            isUAV = 0;

            matchPulseT(match, t2, Indexq, nIndex, UAVtypes[i].pulseT, UAVtypes[i].nPulseT, UAVtypes[i].pulseTErr);

            for (j = 0; j < nIndex; j++)
            {
                if(j>MaxNumVidT-1)
                    break;
                meetPulseTCount = 0;
                for (k = 0; k < MaxNumVidT; k++)
                {
                    if (match[j][k] > 0)
                    {
                        meetPulseTCount++;
                    }
                }
                if (meetPulseTCount > 0 && meetPulseTCount >= UAVtypes[i].hoppCnt)
                {
                    isUAV = 1;
                    break;
                }
            }
            if (isUAV)
            {
                nIndex = 0;
                for (k = 0; k < MaxNumVidT; k++)
                {
                    if (match[j][k] >= 0)
                    {
                        Indexq[nIndex] = match[j][k];
                        nIndex++;
                    }
                }
            }
        }

        if (isUAV)
        {
            minSnr = colSnr[Indexq[0]];
            for (j = 0; j < nIndex; j++)
            {
                if (colSnr[Indexq[j]] < minSnr)
                    minSnr = colSnr[Indexq[j]];
                if (colSnr[Indexq[j]] < UAVtypes[i].SNR)
                {
                    isUAV = 0;
                    break;
                }
            }
        }

        if (isUAV)
        {
            (*aBlkRow).uavIndex = i;
            (*aBlkRow).antIndex = antIndex;
            (*aBlkRow).colIndex = colIndex;
            (*aBlkRow).snr = minSnr;
            (*aBlkRow).nCol = 0;
            (*aBlkRow).meanColAmp = 0.0;
            count = 0;
            for (j = 0; j < nIndex; j++)
            {
                (*aBlkRow).q1[j] = q1[Indexq[j]];
                (*aBlkRow).w[j] = w[Indexq[j]];
                for (k = (*aBlkRow).q1[j]; k < (*aBlkRow).q1[j] + (*aBlkRow).w[j]; k++)
                {
                    (*aBlkRow).meanColAmp += s[k];
                }
                count += (*aBlkRow).w[j];
            }
            (*aBlkRow).meanColAmp /= (float)count;
            (*aBlkRow).nw = nIndex;
            free(Indexq);
            free(t2);
            
            for(i=0;i<MaxVidPulse;i++)
                free(match[i]);
            free(match);
            
            return 1;
        }
    }
    free(Indexq);
    free(t2);
    
    for(i=0;i<MaxVidPulse;i++)
        free(match[i]);
    free(match);
    
    return 0;
}

//图传列检测
void uavRowDetect(
    struct BlockRow *allBlkRow, int *nBlkRow, float ***specMat, float ***filtedAmp, int nRows, float cenFreq, float dT, float dF, struct DroneLib *UAVtypes, int nUAV, int *vidSpan)
{
    int i, j, k;
    int ii, jj, kk;

    float         thresh;
    int           nw;
    struct BlockRow aBlkRow;
    int           nqMeet = 0;
    char          isqMeet = 0;
    char          isNewCol = 0;
    char          isNewq1w = 0;
    int           id = 0;
    
    static float DT = 0.1;
    static int   MaxIter = 20; // 10
    static int   qMeetTimes = 2;
    static int   Delt = 2; // dingyu, 20230202 // Delt = 1;
    int   MinW = 0.7f/dT;
    int   q1IndexErr = 0.5f/dT;

    float *colVecFilted = (float *)malloc(nRows * sizeof(float)), *colVec = (float *)malloc(nRows * sizeof(float));
    int *q1 = (int *)malloc(nRows * sizeof(int));
    int *w = (int *)malloc(nRows * sizeof(int));
    float *colMeanAmp = (float *)malloc(nRows * sizeof(float));
    float *colSnr = (float *)malloc(nRows * sizeof(float));
    
    *nBlkRow = 0;
    for (i = 0; i < NCh; i++)
    {
        for (j = vidSpan[0]; j < vidSpan[1]; j++)
        {
            for (k = (MaskH + 1) / 2; k < nRows - (MaskH - 1) / 2; k++)
            {
                colVecFilted[k - (MaskH + 1) / 2] = filtedAmp[i][k][j];
                colVec[k - (MaskH + 1) / 2] = specMat[i][k][j];
            }
            thresh = autoThresh(colVecFilted, nRows - MaskH, DT, MaxIter);
            nw = threshold(colVecFilted, nRows - MaskH, thresh, Delt, MinW, q1, w, colSnr);
            for (k = 0; k < nw; k++)
                q1[k] += (MaskH + 1) / 2;
            
            if (vidBlocksDetect(&aBlkRow, colVec, nRows-MaskH, q1, w, nw, colSnr, i, j, cenFreq, dT, dF, UAVtypes, nUAV))
            {                
                isqMeet = 0;

                int q1MeetNum[MaxPulseInGroup];
                int maxNumIndex=-1;
                for (k = 0; k < *nBlkRow; k++)
                {
                    q1MeetNum[k]=0;
                    if (aBlkRow.uavIndex != allBlkRow[k].uavIndex)
                    {
                        continue;
                    }
                    nqMeet = 0;
                    for (ii = 0; ii < allBlkRow[k].nw; ii++)
                    {
                        for (jj = 0; jj < aBlkRow.nw; jj++)
                        {
                            if (fabs(allBlkRow[k].q1[ii] - aBlkRow.q1[jj]) < q1IndexErr)
                            {
                                if(fabs(allBlkRow[k].w[ii] - aBlkRow.w[jj]) < q1IndexErr)
                                    nqMeet++;
                            }
                        }
                    }
                    
                    if (nqMeet >= qMeetTimes) 
                    {
                        q1MeetNum[k]=nqMeet;
                        if(maxNumIndex<0)
                        {
                            maxNumIndex=k;
                        }
                        else if(q1MeetNum[maxNumIndex]<nqMeet)
                        {
                            maxNumIndex=k;
                        }
                    }
                }

                if(maxNumIndex>-1)
                {
                    k=maxNumIndex;
                    isqMeet = 1;
                    allBlkRow[k].inCh[aBlkRow.antIndex] = 1;

                    isNewCol = 1;
                    for (ii = 0; ii < allBlkRow[k].nCol; ii++)
                    {
                        if (allBlkRow[k].allColIndex[ii] == aBlkRow.colIndex)
                        {
                            isNewCol = 0;
                            break;
                        }
                    }
                    if (isNewCol)
                    {
                        if (allBlkRow[k].allColIndex[allBlkRow[k].nCol - 1] < aBlkRow.colIndex)
                        {
                            allBlkRow[k].allColIndex[allBlkRow[k].nCol] = aBlkRow.colIndex;
                            allBlkRow[k].nCol++;
                        }
                        else
                        {
                            for (ii = 0; ii < allBlkRow[k].nCol; ii++)
                            {
                                if (allBlkRow[k].allColIndex[ii] > aBlkRow.colIndex)
                                {
                                    for (jj = allBlkRow[k].nCol; jj > ii; jj--)
                                    {
                                        allBlkRow[k].allColIndex[jj] = allBlkRow[k].allColIndex[jj - 1];
                                    }
                                    allBlkRow[k].allColIndex[ii] = aBlkRow.colIndex;
                                    allBlkRow[k].nCol++;
                                    break;
                                }
                            }
                        }
                    }
                    for (ii = 0; ii < aBlkRow.nw; ii++)
                    {
                        isNewq1w = 1;
                        for (jj = 0; jj < allBlkRow[k].nw; jj++)
                        {
                            if (aBlkRow.q1[ii] < allBlkRow[k].q1[jj])
                            {
                                if (aBlkRow.q1[ii] + aBlkRow.w[ii] > allBlkRow[k].q1[jj])
                                {
                                    isNewq1w = 0;
                                    break;
                                }
                            }
                            else
                            {
                                if (aBlkRow.q1[ii] < allBlkRow[k].q1[jj] + allBlkRow[k].w[jj])
                                {
                                    isNewq1w = 0;
                                    break;
                                }
                            }
                        }
                        if (isNewq1w)
                        {
                            if (allBlkRow[k].q1[allBlkRow[k].nw - 1] < aBlkRow.q1[ii])
                            {
                                allBlkRow[k].q1[allBlkRow[k].nw] = aBlkRow.q1[ii];
                                allBlkRow[k].w[allBlkRow[k].nw] = aBlkRow.w[ii];
                                allBlkRow[k].nw++;
                            }
                            else
                            {
                                for (jj = 0; jj < allBlkRow[k].nw; jj++)
                                {
                                    if (allBlkRow[k].q1[jj] > aBlkRow.q1[ii])
                                    {
                                        for (kk = allBlkRow[k].nw; kk > jj; kk--)
                                        {
                                            allBlkRow[k].q1[kk] = allBlkRow[k].q1[kk - 1];
                                            allBlkRow[k].w[kk] = allBlkRow[k].w[kk - 1];
                                        }
                                        allBlkRow[k].q1[jj] = aBlkRow.q1[ii];
                                        allBlkRow[k].w[jj] = aBlkRow.w[ii];
                                        allBlkRow[k].nw++;
                                        break;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (allBlkRow[k].meanColAmp < aBlkRow.meanColAmp)
                            {
                                allBlkRow[k].q1[jj] = aBlkRow.q1[ii];
                                allBlkRow[k].w[jj] = aBlkRow.w[ii];
                            }
                        }
                    }
                    if (allBlkRow[k].meanColAmp < aBlkRow.meanColAmp)
                    {
                        allBlkRow[k].colIndex = aBlkRow.colIndex;
                        allBlkRow[k].meanColAmp = aBlkRow.meanColAmp;
                        allBlkRow[k].snr = aBlkRow.snr;
                    }
                }

                if (!isqMeet && *nBlkRow < MaxDrone * NCh)
                {
                    allBlkRow[*nBlkRow].antIndex = aBlkRow.antIndex;
                    allBlkRow[*nBlkRow].colIndex = aBlkRow.colIndex;
                    allBlkRow[*nBlkRow].uavIndex = aBlkRow.uavIndex;
                    for (k = 0; k < NCh; k++)
                    {
                        if (k == aBlkRow.antIndex)
                            allBlkRow[*nBlkRow].inCh[k] = 1;
                        else
                            allBlkRow[*nBlkRow].inCh[k] = 0;
                    }
                    allBlkRow[*nBlkRow].nw = aBlkRow.nw;
                    allBlkRow[*nBlkRow].meanColAmp = aBlkRow.meanColAmp;
                    for (k = 0; k < aBlkRow.nw; k++)
                    {
                        allBlkRow[*nBlkRow].q1[k] = aBlkRow.q1[k];
                        allBlkRow[*nBlkRow].w[k] = aBlkRow.w[k];
                    }

                    allBlkRow[*nBlkRow].nCol = 1;
                    allBlkRow[*nBlkRow].allColIndex[0] = aBlkRow.colIndex;
                    allBlkRow[*nBlkRow].id = id;
                    allBlkRow[*nBlkRow].snr = aBlkRow.snr;
                    id++;
                    (*nBlkRow)++;
                }
            }
        }
    }

    free(colVecFilted);
    free(colVec);
    free(q1);
    free(w);
    free(colMeanAmp);
    free(colSnr);

}

//图块信息生成脉冲信号参数
int genUavPulse(
    struct blockGroup *droneBlock, struct BlockRow *allBlkRow, int nBlkRow, float ***specMat, int nRows, int nCols, float cenFreq, float dT, float dF, struct DroneLib *UAVtypes)
{
    int i, j, k, ii ,jj;
    
    int nBlock=0;

    int   uavIndex = 0, cenCol = 0;
    float colAmp;
    float pulseFreq;
    int   freqIndexL = 0, freqIndexR = 0, IndexL = 0, IndexR = 0;
    float meanColAmp[NFFT / N] = {0, 0,};
    float minBW=0.0f;

    int   uavCh = 0;
    int   count = 0;

    static float  ampErr = 300.0f;

    for (i = 0; i < nBlkRow; i++)
    {
        uavIndex = allBlkRow[i].uavIndex;
        droneBlock[nBlock].index = uavIndex;
        if (UAVtypes[uavIndex].isFixedFreq)
        {
            droneBlock[nBlock].bw = UAVtypes[uavIndex].pulseBW[0];
            cenCol = allBlkRow[i].colIndex;

            colAmp = -1000.0f;
            for (j = 0; j < NCh; j++)
            {
                if (allBlkRow[i].inCh[j] && allBlkRow[i].meanColAmp > colAmp)
                {
                    uavCh = j;
                    colAmp = allBlkRow[i].meanColAmp;
                }
            }

            colAmp = 0.0;
            for (j = 0; j < allBlkRow[i].nCol; j++)
            {
                meanColAmp[j] = 0;
                count = 0;
                for (k = 0; k < allBlkRow[i].nw; k++)
                {
                    for (ii = allBlkRow[i].q1[k]; ii < allBlkRow[i].q1[k] + allBlkRow[i].w[k]; ii++)
                    {
                        meanColAmp[j] += specMat[uavCh][ii][allBlkRow[i].allColIndex[j]];
                        count++;
                    }
                }
                if (count > 0)
                    meanColAmp[j] /= (float)count;
                colAmp += meanColAmp[j];
            }

            pulseFreq = cenFreq + cenCol * dF - 0.5 * nCols*dF;
            for (j = 0; j < UAVtypes[uavIndex].nfreq; j++)
            {
                if (pulseFreq <= UAVtypes[uavIndex].freqPoints[j] + UAVtypes[uavIndex].pulseBW[0] / 2)
                    break;
            }

            if (j < UAVtypes[uavIndex].nfreq - 1 && pulseFreq > UAVtypes[uavIndex].freqPoints[j + 1] - UAVtypes[uavIndex].pulseBW[0] / 2)
            {
                colAmp = 0;
                for (k = 0; k < allBlkRow[i].nCol; k++)
                {
                    pulseFreq = cenFreq+ allBlkRow[i].allColIndex[k] * dF - 0.5 * nCols*dF;
                    if (pulseFreq > UAVtypes[uavIndex].freqPoints[j] - UAVtypes[uavIndex].pulseBW[0] / 2 && pulseFreq <= UAVtypes[uavIndex].freqPoints[j] + UAVtypes[uavIndex].pulseBW[0] / 2)
                        colAmp += meanColAmp[k];
                    if (pulseFreq > UAVtypes[uavIndex].freqPoints[j + 1] - UAVtypes[uavIndex].pulseBW[0] / 2 && pulseFreq <= UAVtypes[uavIndex].freqPoints[j + 1] + UAVtypes[uavIndex].pulseBW[0] / 2)
                        colAmp -= meanColAmp[k];
                }
                if (colAmp < 0)
                    j++;
            }
            for (k = 0; k < allBlkRow[i].nw; k++)
                droneBlock[nBlock].freq[k] = UAVtypes[uavIndex].freqPoints[j];

            freqIndexL = (droneBlock[nBlock].freq[0] - droneBlock[nBlock].bw / 2 - (cenFreq - 0.5 * nCols*dF))/dF;
            freqIndexR = (droneBlock[nBlock].freq[0] + droneBlock[nBlock].bw / 2 - (cenFreq - 0.5 * nCols*dF))/dF;

            if (freqIndexR >= NFFT / N - 1)
                freqIndexR = NFFT / N - 1;
            if (freqIndexL < 0)
                freqIndexL = 0;
        }
        else
        {
            minBW = UAVtypes[uavIndex].pulseBW[0];
            colAmp = -1000.0;
            for (j = 0; j < NCh; j++)
            {
                if (allBlkRow[i].inCh[j] && allBlkRow[i].meanColAmp > colAmp)
                {
                    uavCh = j;
                    colAmp = allBlkRow[i].meanColAmp;
                }
            }

            IndexL = allBlkRow[i].allColIndex[0];
            IndexR = allBlkRow[i].allColIndex[0];
            for (j = 0; j < allBlkRow[i].nCol; j++)
            {
                if (allBlkRow[i].allColIndex[j] < IndexL)
                    IndexL = allBlkRow[i].allColIndex[j];
                if (allBlkRow[i].allColIndex[j] > IndexR)
                    IndexR = allBlkRow[i].allColIndex[j];
            }

            if ((IndexR - IndexL + 1) * dF < UAVtypes[uavIndex].pulseBW[0])
            {
                freqIndexR = IndexR;
                freqIndexL = IndexL;
            }
            else
            {
                for (j = IndexL; j < IndexR; j++)
                {
                    meanColAmp[j] = 0;
                    count = 0;
                    for (k = 0; k < allBlkRow[i].nw; k++)
                    {
                        for (ii = allBlkRow[i].q1[k]; ii < allBlkRow[i].q1[k] + allBlkRow[i].w[k]; ii++)
                        {
                            meanColAmp[j] += specMat[uavCh][ii][j];
                            count++;
                        }
                    }
                    if (count > 0)
                        meanColAmp[j] /= (float)count;
                }

                cenCol = allBlkRow[i].colIndex;
                float sumBWamp[5] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
                int   indexBl[5] = { 0, 0, 0, 0, 0 };
                int   indexBr[5] = { 0, 0, 0, 0, 0 };
                for (j = 0; j < UAVtypes[uavIndex].nPulseBW; j++)
                {
                    sumBWamp[j] = 0.0;
                    float sumAmp = 0.0;
                    int   nColBW = UAVtypes[uavIndex].pulseBW[j] / dF;
                    for (k = IndexL; k < IndexR + 1; k++)
                    {
                        sumAmp = 0.0;
                        int colCount = 0;
                        for (int p = 0; p < nColBW; p++)
                        {
                            if (k + p < IndexR + 1)
                            {
                                sumAmp += meanColAmp[k + p];
                                colCount++;
                            }
                        }
                        sumAmp /= (float)colCount;
                        if (sumAmp > sumBWamp[j])
                        {
                            sumBWamp[j] = sumAmp;
                            indexBl[j] = k;
                            indexBr[j] = k + colCount;
                        }
                    }
                }

                k = 0;
                for (j = 1; j < UAVtypes[uavIndex].nPulseBW; j++)
                {
                    if (fabs(sumBWamp[j] - sumBWamp[0]) < ampErr)
                        k = j;
                }

                freqIndexL = indexBl[k];
                freqIndexR = indexBr[k];

            }

            droneBlock[nBlock].bw = (freqIndexR + 1 - freqIndexL) * dF;
            droneBlock[nBlock].freq[0] = cenFreq - 0.5 * nCols*dF + (freqIndexL + freqIndexR) / 2.0 * dF;
            for (j = 1; j < allBlkRow[i].nw; j++)
                droneBlock[nBlock].freq[j] = droneBlock[nBlock].freq[0];
        }

		// dingyu, 20230117
		if (droneBlock[nBlock].bw < 2.5 * dF)
			continue;

        droneBlock[nBlock].id = allBlkRow[i].id;
        droneBlock[nBlock].n = allBlkRow[i].nw;
        for (j = 0; j < NCh; j++)
        {
            droneBlock[nBlock].inCh[j] = allBlkRow[i].inCh[j];
        }
        for (j = 0; j < allBlkRow[i].nw; j++)
        {
            droneBlock[nBlock].burstTime[j] = allBlkRow[i].q1[j] * dT;
            droneBlock[nBlock].width[j] = allBlkRow[i].w[j] * dT;
            
            for (k = 0; k < NCh; k++)
            {
                droneBlock[nBlock].amp[k][j] =0.0f;
                
                for (ii = allBlkRow[i].q1[j]; ii < allBlkRow[i].q1[j] + allBlkRow[i].w[j]; ii++)
                {
                    for (jj = freqIndexL; jj < freqIndexR + 1; jj++)
                    {
                        droneBlock[nBlock].amp[k][j] += specMat[k][ii][jj];
                    }
                }
                droneBlock[nBlock].amp[k][j] /= (freqIndexR + 1 - freqIndexL) * allBlkRow[i].w[j];
            }
        }

        nBlock++;

        if (nBlock >= MaxDrone)
            break;
    }
    
    return nBlock;
}


//计算距离
void calcUavPulse(struct blockGroup *droneBlock, int nBlock,  float gain)
{
    float maxAmp=-10000.0f;
    float range = 0.0;


    for (int i = 0; i < nBlock; i++)
    {
        for (int j = 0; j < droneBlock[i].n; j++)
        {
            maxAmp=-10000.0f;
            for (int k = 0; k < NCh; k++)
            {
                if(droneBlock[i].amp[k][j]>maxAmp)
                    maxAmp=droneBlock[i].amp[k][j];
            }

            droneBlock[i].dist[j] = amp2dist(maxAmp, droneBlock[i].freq[j], gain);
            range += droneBlock[i].dist[j];
        }
        droneBlock[i].range = range / droneBlock[i].n;
    }
}

//打印信息
void printDroneBlock(struct blockGroup *droneBlock, int nBlock, struct DroneLib *UAVtypes)
{
    int  i, j;
    printf("**********************drone*********************\n");
    for (i = 0; i < nBlock; i++)
    {
        printf("*******************************************\n");
        printf("ID=%d,%s\n", droneBlock[i].id, UAVtypes[droneBlock[i].index].name);
        printf("cenFreq=%.1f,bandWidth=%.1f\n", droneBlock[i].freq[i], droneBlock[i].bw);
        printf("inCh:");
        for (j = 0; j < NCh; j++)
        {
            if (droneBlock[i].inCh[j])
                printf("%d,", j + 1);
        }
        printf("\n");
        for (j = 0; j < droneBlock[i].n; j++)
        {
            printf("burstTime=%.1f,width=%2.1f,bw=%.1f,dist=%.0f\n", droneBlock[i].burstTime[j], droneBlock[i].width[j], droneBlock[i].bw, droneBlock[i].dist[j]);
        }
    }
    printf("*******************************************\n");
}

//图传检测主函数
void downLoadSniffer( struct blockGroup *droneBlock, int *nBlock, float ***specMat, int nRows, int nCols, float cenFreq, float gain, float dT, float dF, struct DroneLib *UAVtypes, int nUav, int *vidSpan)
{

    struct BlockRow *allBlkRow = (struct BlockRow *)malloc(MaxDrone * NCh * sizeof(struct BlockRow));
    int            nBlkRow = 0;

    float ***filtedAmp = createMat(NCh, nRows, nCols);
    
    for (int i = 0; i < NCh; i++)
    {
        conv2(*(specMat + i), nRows, nCols, *(filtedAmp + i), Mask, MaskH, MaskW, vidSpan);
    }

    uavRowDetect(allBlkRow, &nBlkRow, specMat, filtedAmp, nRows, cenFreq, dT, dF, UAVtypes, nUav, vidSpan);
    
    *nBlock=genUavPulse(droneBlock, allBlkRow, nBlkRow, specMat, nRows, nCols, cenFreq, dT, dF, UAVtypes);

    calcUavPulse(droneBlock, *nBlock, gain);

    free(allBlkRow);
    freeMat(filtedAmp, NCh, nRows);
}


//图传检测主函数，二维卷积作为输入参数的版本
void downLoadSniffer_noFilt( struct blockGroup *droneBlock, int *nBlock, float ***downMat, int nRows, int nCols, float cenFreq, float gain, float dT, float dF, struct DroneLib *UAVtypes, int nUav, int *vidSpan)
{

    struct BlockRow *allBlkRow = (struct BlockRow *)malloc(MaxDrone * NCh * sizeof(struct BlockRow));
    int            nBlkRow = 0;

    uavRowDetect(allBlkRow, &nBlkRow, downMat, downMat, nRows, cenFreq, dT, dF, UAVtypes, nUav, vidSpan);
    
    *nBlock=genUavPulse(droneBlock, allBlkRow, nBlkRow, downMat, nRows, nCols, cenFreq, dT, dF, UAVtypes);

    calcUavPulse(droneBlock, *nBlock, gain);

    free(allBlkRow);
}


