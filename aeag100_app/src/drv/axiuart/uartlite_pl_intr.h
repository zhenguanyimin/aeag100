/********************MILIANKE**************************

 *Company:Liyang Milian Electronic Technology Co., Ltd
 *Technical forum:www.uisrc.com
 *Taobao: https://milianke.taobao.com
 *Create Date: 2020/12/01
 *Module Name:uartlite_pl_intr
 *Copyright: Copyright (c) milianke
 *Revision: 1.1
 *Description:

****************************************************/

#include "xparameters.h"
#include "xuartlite.h"
#include "xil_exception.h"
#include "xuartlite_l.h"
#include "xscugic.h"
#include "xil_printf.h"

#define UARTLITE_DEVICE_ID_0 XPAR_AXI_UARTLITE_0_DEVICE_ID
#define UARTLITE_DEVICE_ID_1	  XPAR_AXI_UARTLITE_1_DEVICE_ID
//#define UARTLITE_DEVICE_ID_2      XPAR_AXI_UARTLITE_2_DEVICE_ID
//#define UARTLITE_DEVICE_ID_3	  XPAR_AXI_UARTLITE_3_DEVICE_ID
//#define UARTLITE_DEVICE_ID_4      XPAR_AXI_UARTLITE_4_DEVICE_ID
//
#define UARTLITE_IRPT_INTR_0 XPAR_FABRIC_UARTLITE_0_VEC_ID
#define UARTLITE_IRPT_INTR_1	  XPAR_FABRIC_UARTLITE_1_VEC_ID
//#define UARTLITE_IRPT_INTR_2	  XPAR_FABRIC_UARTLITE_2_VEC_ID
//#define UARTLITE_IRPT_INTR_3	  XPAR_FABRIC_UARTLITE_3_VEC_ID
//#define UARTLITE_IRPT_INTR_4	  XPAR_FABRIC_UARTLITE_4_VEC_ID

int UartLite_Init(XUartLite *UartLiteInstPtr, u16 UartLiteDeviceId, XUartLite_Handler FuncPtr);
int UartLite_setup_IntrSystem(XScuGic *IntcInstancePtr, XUartLite *UartLiteInstPtr, u16 UartLiteIntrId);
void UartLiteDisableIntrSystem(XScuGic *IntrInstancePtr, u16 UartLiteIntrId);
