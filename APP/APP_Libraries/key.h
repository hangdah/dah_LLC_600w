/**
 * @file key.h
 * @brief 矩阵键盘驱动头文件
 * @date 2025
 * @author da
 *
 * 3x3矩阵键盘配置:
 * - 列 (输入): GPIO12, GPIO13, GPIO14
 * - 行 (输出): GPIO48, GPIO49, GPIO50
 * - 支持9个按键 (KEY1-KEY9) + KEY_UNPRESS
 */

#ifndef APP_APP_LIBRARIES_KEY_H_
#define APP_APP_LIBRARIES_KEY_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

/** 行输出控制宏 */
#define KEY_L1_SETL (GpioDataRegs.GPBCLEAR.bit.GPIO48=1)
#define KEY_L2_SETL (GpioDataRegs.GPBCLEAR.bit.GPIO49=1)
#define KEY_L3_SETL (GpioDataRegs.GPBCLEAR.bit.GPIO50=1)

#define KEY_L1_SETH (GpioDataRegs.GPBSET.bit.GPIO48=1)
#define KEY_L2_SETH (GpioDataRegs.GPBSET.bit.GPIO49=1)
#define KEY_L3_SETH (GpioDataRegs.GPBSET.bit.GPIO50=1)

/** 列输入读取宏 */
#define KEY_H1 (GpioDataRegs.GPADAT.bit.GPIO12)
#define KEY_H2 (GpioDataRegs.GPADAT.bit.GPIO13)
#define KEY_H3 (GpioDataRegs.GPADAT.bit.GPIO14)

/** 按键代码定义 */
#define KEY1_PRESS 1
#define KEY2_PRESS 2
#define KEY3_PRESS 3
#define KEY4_PRESS 4
#define KEY5_PRESS 5
#define KEY6_PRESS 6
#define KEY7_PRESS 7
#define KEY8_PRESS 8
#define KEY9_PRESS 9
#define KEY_UNPRESS 0

void Key_Init();
char Key_Scan(char mode);



#endif /* APP_APP_LIBRARIES_KEY_H_ */
