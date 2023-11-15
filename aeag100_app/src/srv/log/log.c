
#include "log.h"
#include "xil_printf.h"
#include <stdarg.h>
#include "FreeRTOS.h"
#include "task.h"

#include "../../drv/axiuart/sys_intr.h"
#include "../cli/serial.h"
STATIC log_level_t s_log_lvl = LL_NONE;

void log_set_lvl(log_level_t lvl)
{
    s_log_lvl = lvl;
}

log_level_t log_get_lvl_num(void)
{
    return s_log_lvl;
}

static uint8_t uOutputChl = LOG_PS_UART0;
void SetSwitchOutputChl(uint8_t value)
{
    uOutputChl = value;
}

const char *log_get_lvl_str(void)
{
    const char *lvl = NULL;

    switch (s_log_lvl)
    {
    case LL_ANY:
        lvl = "ANY";
        break;
    case LL_VERB:
        lvl = "VERB";
        break;
    case LL_INFO:
        lvl = "INFO";
        break;
    case LL_DEBUG:
        lvl = "DEBUG";
        break;
    case LL_ERROR:
        lvl = "ERROR";
        break;
    case LL_PRINTF:
        lvl = "PRINTF";
        break;
    default:
        lvl = "NONE";
        break;
    }

    return lvl;
}

#define RECV_MAX 256
static uint8_t uartbuf[RECV_MAX + 100];
static uint16_t uartbufcnt = 0;
int LogUartPrint1(log_level_t lvl, char *pSendBuf, u32 len)
{
    char strTime[8] = {0};
    if (lvl != LL_PRINTF)
    {
        unsigned int time = (unsigned int)xTaskGetTickCount();
        sprintf(strTime, "%u:", time);
        memcpy(uartbuf + uartbufcnt, strTime, strlen(strTime));
        uartbufcnt += strlen(strTime);
    }

    memcpy(uartbuf + uartbufcnt, pSendBuf, len);
    uartbufcnt += len;
    if (uartbufcnt > RECV_MAX)
    {
        //		XUartLite_Send(&gLogUart , uartbuf , uartbufcnt );
        if (uOutputChl == LOG_Pl_UART2)
        {
            LogUartPrint2(uartbuf, uartbufcnt);
        }
        else
        {
            UartLite_SendData(0, uartbuf, uartbufcnt);
        }

        uartbufcnt = 0;
    }

    return 0;
}

void log_record(log_level_t lvl, char *fmt, ...)
{
    char log_buf[128] = {0};

    if (lvl < s_log_lvl)
    {
        return;
    }

    va_list args;
    va_start(args, fmt);
    vsnprintf(log_buf, sizeof(log_buf), fmt, args);
    if (uOutputChl == LOG_PS_UART0)
    {
        xil_printf("%s", log_buf);
    }
    else
    {
        LogUartPrint1(lvl, log_buf, strlen(log_buf));
    }
    va_end(args);
}

void log_dump(uint32_t base_addr, uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;

    if ((NULL == buf) || (0 == len))
    {
        return;
    }

    for (i = 0; i < len; i++)
    {
        if (i % 16 == 0)
        {
            log_record(LL_PRINTF, "%08x:", (base_addr + i));
        }

        if ((i + 1) % 16 == 0)
        {
            log_record(LL_PRINTF, " %02x\r\n", buf[i]);
        }
        else
        {
            log_record(LL_PRINTF, " %02x", buf[i]);
        }
    }
    log_record(LL_PRINTF, "\r\n");
}
