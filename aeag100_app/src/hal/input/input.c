#include "input.h"
/**
 * 读取引脚电平
 * @param name 引脚名字
 * @return	1: 高电平     0: 低电平
 */
uint8_t Input_GetValue(INPUT_NAME name)
{
	return (uint8_t)axi_read_data(name);
}
