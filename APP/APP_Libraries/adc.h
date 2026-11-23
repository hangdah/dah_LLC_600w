/**
 * @file adc.h
 * @brief 用于电压/电流采样的ADC驱动头文件
 * @date 2025
 * @author da
 *
 * ADC配置:
 * - SEQ1: 用于控制环的4通道自动转换
 *   * ADCRESULT0: Vout (ADCINA0)
 *   * ADCRESULT1: IL (ADCINA1)
 *   * ADCRESULT2: Vin (ADCINB0)
 *   * ADCRESULT3: Iout (ADCINB1)
 * - 由ePWM SOCA触发实现同步采样
 */

#ifndef APP_APP_LIBRARIES_ADC_H_
#define APP_APP_LIBRARIES_ADC_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

/** ADC模块时钟预分频器 (HSPCLK/(2*ADC_MODCLK)) */
#define ADC_MODCLK 3

void ADC_Init(void);
Uint16 Read_ADC_CH0_Value(void);
interrupt void  adc_isr(void);



#endif /* APP_APP_LIBRARIES_ADC_H_ */
