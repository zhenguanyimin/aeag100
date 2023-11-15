/**
 * Copyright (C), Autel Tech. Co., Ltd.
 * @brief   Realize chip function configuration
 * @file    interrupt_handle.h
 * @author  X22012
 * @date    2022-05-15
 *
 * -History:
 *      -# author : X22012
 *         date   : 2022-05-15
 *         Version: V1.0
 *         details: Created
 */

#ifndef INTERRUPT_HANDLE_H_
#define INTERRUPT_HANDLE_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <xparameters.h>
#include <xscugic.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define INTC_DEVICE_ID XPAR_PS7_SCUGIC_0_DEVICE_ID

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

int IntcInitFunction(uint16_t DeviceId);

void *IntcGetGicInst(void);
void IntcTypeSetup(XScuGic *InstancePtr, int intId, int intType);

#endif /* INTERRUPT_HANDLE_H_ */
