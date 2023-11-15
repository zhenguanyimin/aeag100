#include "switch_filter.h"
#include "../../drv/axiuart/sys_intr.h"
#include "../../srv/log/log.h"
#include "../../srv/watch_srv/watch_srv.h"

void SimulaAdp_SendSerninCmd(uint8_t cmdNum)
{
	static char cmd[8] = "SERNIN";
	sprintf(cmd + 6, "%d", cmdNum);
	//	snprintf(&cmd[6], 1, "%d", cmdNum);
	LOG_DEBUG("%s \r\n", cmd);
	SimulaAdp_SendData((uint8_t *)cmd, strlen((const char *)cmd));
}

void SimulaAdp_SendAdrf5250md(uint8_t cmdNum)
{
	static char cmd0[15] = "ADRF5250_RF";
	//	snprintf(&cmd0[11], 1, "%d", cmdNum);
	sprintf(cmd0 + 11, "%d\n", cmdNum);
//	LOG_DEBUG("%s \r\n", cmd0);
	SimulaAdp_SendData((uint8_t *)cmd0, strlen((const char *)cmd0));
}

void SimulaAdp_SendData(uint8_t *buff, uint32_t len)
{
	UartLite_SendData(WATCH_UARTLITE_NUM, buff, len);
}
