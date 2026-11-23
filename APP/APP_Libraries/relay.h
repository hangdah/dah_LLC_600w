/**
 * @file relay.h
 * @brief 继电器控制头文件
 * @date 2025
 * @author da
 *
 * 继电器配置:
 * - 连接到GPIO15
 * - 高电平有效 (置位以开启)
 * - 用于输出功率控制
 */

#ifndef APP_APP_LIBRARIES_RELAY_H_
#define APP_APP_LIBRARIES_RELAY_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define RELAY_ON  (GpioDataRegs.GPASET.bit.GPIO15=1)
#define RELAY_OFF (GpioDataRegs.GPACLEAR.bit.GPIO15=1)

void Relay_Init();



#endif /* APP_APP_LIBRARIES_RELAY_H_ */
