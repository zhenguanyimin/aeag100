#ifndef ALINK_MSG_PROC_H
#define ALINK_MSG_PROC_H
#include "alink_msg_common.h"

#define RECV_MAX_BUFF 1024
#define ANS_RECV_MAX_BUFF 10

typedef struct alink_msg
{
    alink_msg_head_t msg_head;     //  message header
    uint8_t buffer[RECV_MAX_BUFF]; //
    uint16_t checksum;             ///< Checksum
} alink_msg_t;

typedef struct alink_ans
{
    uint16_t msg_len;                  //
    uint8_t buffer[ANS_RECV_MAX_BUFF]; ///
} alink_ans_t;

typedef struct alink_msg_point
{
    uint16_t msg_len;                  //
    uint8_t buffer[ANS_RECV_MAX_BUFF]; ///
} alink_msg_point_t;

/**
 *  @b Description
 *  @n
 *      Init alink message process module.
 *
 *  @retval
 *      Success    - 0
 *      Fail       - others
 */
int32_t AlinkMsgProc_Init(void);

/*
 *  send-receive data
 * return
 */
int32_t AlinkMsgSendRecv(void *pUartpl, alink_msg_t *pMsg, alink_ans_t *pAns_msg, uint32_t timeout);
void AlinkMsg_UartLiteRecvHandler(void *CallBackRef, uint32_t EventData);
uint8_t *Get_RevBuffPoint();
void test();

#endif /* ALINK_MSG_PROC_H */
