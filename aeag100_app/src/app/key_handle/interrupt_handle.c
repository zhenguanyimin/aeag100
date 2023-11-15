/**
 * Copyright (C), Autel Tech. Co., Ltd.
 * @brief   Realize chip function configuration
 * @file    interrupt_handle.c
 * @author  X22012
 *
 * -History:
 *      -# author : X22012
 *         date   : 2022-05-15
 *         Version: V1.0
 *         details: Created
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "interrupt_handle.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
#include <stdio.h>
#include "xscugic.h"
#include "xil_printf.h"
#include "xil_exception.h"
#include "../../app/key_handle/key_handle.h"
#include "../../hal/hal_ad9361/zynq_bram.h"

#define INT_CFG0_OFFSET 0x00000C00

// Parameter definitions
#define KEY4_SW1_INT_ID 65
#define KEY1_SW4_INT_ID 64
#define ROTATE_SW_INT_ID 66
#define ROTATE_CLOCKWISE_INT_ID 67
#define ROTATE_REVERSE_INT_ID 68

#define INT_TYPE_RISING_EDGE 0x03
#define INT_TYPE_HIGHLEVEL 0x01
#define INT_TYPE_MASK 0x03

/* Definitions for peripheral AXI_GPIO_0_WRITE_READ */
#define XPAR_AXI_GPIO_0_WRITE_READ_BASEADDR 0x41200000
#define XPAR_AXI_GPIO_0_WRITE_READ_HIGHADDR 0x4120FFFF
#define XPAR_AXI_GPIO_0_WRITE_READ_DEVICE_ID 0
#define XPAR_AXI_GPIO_0_WRITE_READ_INTERRUPT_PRESENT 0
#define XPAR_AXI_GPIO_0_WRITE_READ_IS_DUAL 0

/* Definitions for peripheral AXI_GPIO_1_0165_INTERRUPT */
#define XPAR_AXI_GPIO_1_0165_INTERRUPT_BASEADDR 0x41210000
#define XPAR_AXI_GPIO_1_0165_INTERRUPT_HIGHADDR 0x4121FFFF
#define XPAR_AXI_GPIO_1_0165_INTERRUPT_DEVICE_ID 1
#define XPAR_AXI_GPIO_1_0165_INTERRUPT_INTERRUPT_PRESENT 1
#define XPAR_AXI_GPIO_1_0165_INTERRUPT_IS_DUAL 0

/* Definitions for peripheral AXI_GPIO_2_0165_MODEL */
#define XPAR_AXI_GPIO_2_0165_MODEL_BASEADDR 0x41220000
#define XPAR_AXI_GPIO_2_0165_MODEL_HIGHADDR 0x4122FFFF
#define XPAR_AXI_GPIO_2_0165_MODEL_DEVICE_ID 2
#define XPAR_AXI_GPIO_2_0165_MODEL_INTERRUPT_PRESENT 0
#define XPAR_AXI_GPIO_2_0165_MODEL_IS_DUAL 0

extern XScuGic xInterruptController;

void axi_write_data1(unsigned int addr, int dat)
{
	unsigned int *LocalAddr = (unsigned int *)(AXI_BASE_ADDR + (addr << 2));
	*LocalAddr = dat;
}

int axi_read_data1(unsigned int addr)
{
	return *(volatile unsigned int *)(AXI_BASE_ADDR + (addr << 2));
}

void KEY1_intr_Handler(void *param)
{
	//	axi_write_data1(0x61, 1);
	//	data_path_out_en(0);
	//	SetAlgorithmStopFlag(1);
	//	KeyHandleTask_Post(eKEY_TASK_HIT);
}

void KEY4_intr_Handler(void *param)
{
	KeyHandleTask_Post(eKEY_TASK_SET);
}

void Rotate_sw_intr_Handler(void *param)
{
	KeyHandleTask_Post(eKEY_TASK_ROTATE_SW);
}

void Rotate_clockwise_intr_Handler(void *param)
{
	KeyHandleTask_Post(eKEY_TASK_ROTATE_CLOCKWISE);
}

void Rotate_reverse_intr_Handler(void *param)
{
	KeyHandleTask_Post(eKEY_TASK_ROTATE_REVERSE);
}

void IntcTypeSetup(XScuGic *InstancePtr, int intId, int intType)
{
	int mask;
	uint32_t nLocalIntId = (uint32_t)intId;

	intType &= INT_TYPE_MASK;
	mask = XScuGic_DistReadReg(InstancePtr, INT_CFG0_OFFSET + (nLocalIntId / 16) * 4);
	mask &= ~(INT_TYPE_MASK << (intId % 16) * 2);
	mask |= intType << ((intId % 16) * 2);
	XScuGic_DistWriteReg(InstancePtr, INT_CFG0_OFFSET + (nLocalIntId / 16) * 4, mask);
}

int IntcInitFunction(u16 DeviceId)
{
	int status;

	// Connect SW1~SW3 interrupt to handler
	status = XScuGic_Connect(&xInterruptController,
							 KEY1_SW4_INT_ID,
							 (Xil_ExceptionHandler)KEY1_intr_Handler,
							 (void *)NULL);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	status = XScuGic_Connect(&xInterruptController,
							 KEY4_SW1_INT_ID,
							 (Xil_ExceptionHandler)KEY4_intr_Handler,
							 (void *)NULL);
	status = XScuGic_Connect(&xInterruptController,
							 ROTATE_SW_INT_ID,
							 (Xil_ExceptionHandler)Rotate_sw_intr_Handler,
							 (void *)NULL);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	status = XScuGic_Connect(&xInterruptController,
							 ROTATE_CLOCKWISE_INT_ID,
							 (Xil_ExceptionHandler)Rotate_clockwise_intr_Handler,
							 (void *)NULL);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	status = XScuGic_Connect(&xInterruptController,
							 ROTATE_REVERSE_INT_ID,
							 (Xil_ExceptionHandler)Rotate_reverse_intr_Handler,
							 (void *)NULL);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Set interrupt type of SW1~SW2 to rising edge
	IntcTypeSetup(&xInterruptController, KEY1_SW4_INT_ID, INT_TYPE_RISING_EDGE);
	IntcTypeSetup(&xInterruptController, KEY4_SW1_INT_ID, INT_TYPE_RISING_EDGE);
	IntcTypeSetup(&xInterruptController, ROTATE_SW_INT_ID, INT_TYPE_RISING_EDGE);
	IntcTypeSetup(&xInterruptController, ROTATE_CLOCKWISE_INT_ID, INT_TYPE_RISING_EDGE);
	IntcTypeSetup(&xInterruptController, ROTATE_REVERSE_INT_ID, INT_TYPE_RISING_EDGE);
	//    XScuGic_SetPriorityTriggerType(&xInterruptController, CHC2442_INT_ID, 0xb0, 0x3);
	//    XScuGic_SetPriorityTriggerType(&xInterruptController, LMX2492_INT_ID, 0xb8, 0x3);
	//    XScuGic_SetPriorityTriggerType(&xInterruptController, AWMF0165_INT_ID, 0xc0, 0x3);
	//    XScuGic_SetPriorityTriggerType(&xInterruptController, RF_CPIE_INT_ID, 0xc8, 0x3);

	// Enable SW1~SW2 interrupts in the controller
	XScuGic_Enable(&xInterruptController, KEY1_SW4_INT_ID);
	XScuGic_Enable(&xInterruptController, KEY4_SW1_INT_ID);
	XScuGic_Enable(&xInterruptController, ROTATE_SW_INT_ID);
	XScuGic_Enable(&xInterruptController, ROTATE_CLOCKWISE_INT_ID);
	XScuGic_Enable(&xInterruptController, ROTATE_REVERSE_INT_ID);

	return XST_SUCCESS;
}

void *IntcGetGicInst(void)
{
	return (void *)&xInterruptController;
}
