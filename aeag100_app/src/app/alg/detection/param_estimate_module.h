#ifndef PARAM_ESTIMATE_MODULE_H_
#define PARAM_ESTIMATE_MODULE_H_

#include "system_param.h"

/* Define */
#define THRESHOLD_MAG (2.0f) //(0.1f)
#define THRESHOLD_TIME (60u)
#define THRESHOLD_FREQ (120u)

#define CONST_1 (40.0f)
#define CONST_2 (92.0f)

/* Function declaration */
unsigned int paramEstiProcessing(float (*stftOut)[SIZE_FFT], detectedPoint *detPoint, signalParameter *sigParam);
unsigned int paramEstiProcessing_V1_1(float (*stftOut)[SIZE_FFT], detectedPoint *detPoint, signalParameter *sigParam, unsigned int *pNumDetPoint);

#endif
