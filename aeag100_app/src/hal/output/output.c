#include "output.h"

/**
 * 给指定的IO口输出指定的value 电平信号；
 */
void GPIO_OutputCtrl(OUTPUT_NAME name, uint8_t value)
{
	axi_write_data(name, value);
}


/**
 * write register of frequency；
 */
void write_register(uint32_t regAddr, uint32_t value)
{
	axi_write_data(regAddr, value);
}
