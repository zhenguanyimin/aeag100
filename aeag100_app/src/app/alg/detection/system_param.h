#ifndef SYSTEM_PARAM_H_
#define SYSTEM_PARAM_H_

/* Define */
#define SAMPLING_RATE_MHZ		(40.0f)
#define SIZE_FFT				(512u)
#define NUM_FFT_PER_FRAME		(1600u)
#define SIZE_STFT_OUTPUT		(SIZE_FFT * NUM_FFT_PER_FRAME)
#define START_FREQUENCY			(1200.0f)

#define FREQ_RESOLUTION_MHZ		(SAMPLING_RATE_MHZ / SIZE_FFT)
#define TIME_RESOLUTION_US		((1.0f/FREQ_RESOLUTION_MHZ)/2.0f)

#define MAX_NUM_POINT			(1024u)
#define MAX_NUM_SIGNAL			(1024u)
#define MAX_NUM_SIGNAL_FINAL	(1024u)


/* Structure */

/*!
 *  @brief   Structure for detected points 
 *
 */
typedef struct detectedPoint_t
{
    unsigned int    id;             /* point ID */
    unsigned int    tidx;           /* point time ID */
    unsigned int    fidx;           /* point frequency ID */
    float           mag;            /* point magnitude */
} detectedPoint;

/*!
 *  @brief    Output list for detSig parameters
 *
 */
typedef struct signalParameter_t
{
    unsigned int    id;           /* signal ID */
    float           acqTime;      /* time of signal acquisition, in us */
    float           freq;         /* signal frequency, in MHz */
    float           BW;           /* signal bandwidth, in MHz */
    float           dur;          /* signal duration, in us */
    float           amp;          /* signal amplitude, in dB */
} signalParameter;

/*!
 *  @brief    structure for pre-processing results
 *
 */
typedef struct pjieguo_t
{
    unsigned int    id;           /* database ID */
    float           acqTime;      /* time of signal acquisition, in us */
    float           freq;         /* signal frequency, in MHz */
    float           BW;           /* signal bandwidth, in MHz */
    float           dur;          /* signal duration, in us */
    float           amp;          /* signal amplitude, in dB */
    int             numTtl;       /* signal occurrence number  */
    float           prt;          /* signal repetition period*/
    float           distance;     /* signal distance, in m */
} pjieguo;

/*!
 *  @brief   Structure for info of stft result over threshold detected points
 *
 */
typedef struct stftPointInfo_t
{
    unsigned int    pointNum;						/* number of points  */
    detectedPoint	pointList[SIZE_STFT_OUTPUT];	/* list of points  */
} stftPointInfo;

/*!
 *  @brief   Structure for info of detected points
 *
 */
typedef struct detectedPointInfo_t
{
    unsigned int    pointNum;						/* number of points  */
    detectedPoint	pointList[MAX_NUM_POINT];		/* list of points  */
} detectedPointInfo;

/*!
 *  @brief   Structure for info of detSig parameters
 *
 */
typedef struct signalParameterInfo_t
{
    unsigned int    signalNum;						/* number of signal  */
    signalParameter	signalList[MAX_NUM_SIGNAL];		/* list of signal  */
} signalParameterInfo;

/*!
 *  @brief   Structure for info of data processing results
 *
 */
typedef struct pjieguoInfo_t
{
	int flagUAV;									/* flag of identified UAV, yes(>0); not(=0) */
	int signalIdxUAV;								/* signal index of identified UAV */
	float outputWarnDistUAV;						/* output warning distance of identified UAV */
    unsigned int    signalNum;						/* number of signal  */
    pjieguo	signalList[MAX_NUM_SIGNAL_FINAL];		/* list of signal  */
} pjieguoInfo;


#endif
