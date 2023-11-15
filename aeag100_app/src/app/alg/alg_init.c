#include "alg_init.h"
#include "detection/detection_interface.h"
/*
 *initialize algorithm
 */
void algorithm_init()
{
}

int32_t algorithm_Run(sSignalDescriptionList *pSignalFeatureList, output_UAV_List *pOutputList)
{

	return 0;
}
int32_t algorithm_Run1(uint8_t *pBuf, uint32_t len, pjieguoInfo *p_pwjieguoInfo)
{
	return RunDetectAlg(pBuf, len, p_pwjieguoInfo);
}

uint8_t AlgorithmStopFlag = 0;
void SetAlgorithmStopFlag(uint8_t value)
{
	AlgorithmStopFlag = value;
}

uint8_t GetAlgorithmStopFlag()
{
	return AlgorithmStopFlag;
}
