#include "control_sampling.h"
#include "control_config.h"
#include "DSP2833x_Device.h"

static struct _ADC_Values LLC_Values;
static Uint16 g_llc_adc_started;
static Uint16 g_llc_adc_valid;

#pragma CODE_SECTION(LLC_ADC_ReadPrevious_And_StartNext, ".TI.ramfunc")
const struct _ADC_Values *LLC_ADC_ReadPrevious_And_StartNext(void)
{
    static float LLC_Temp_V_    = 0.0f;
    static float LLC_Temp_I_    = 0.0f;
    static float LLC_Temp_Vin_  = 0.0f;
    static float LLC_Temp_Iout_ = 0.0f;

    /* 1. 先读取“上一次已经完成”的结果 */
    if (g_llc_adc_started && (AdcRegs.ADCST.bit.INT_SEQ1 == 1))
    {
        float LLC_Temp_V    = (float)(AdcRegs.ADCRESULT0 >> 4);
        float LLC_Temp_I    = (float)(AdcRegs.ADCRESULT1 >> 4);
        float LLC_Temp_Vin  = (float)(AdcRegs.ADCRESULT2 >> 4);
        float LLC_Temp_Iout = (float)(AdcRegs.ADCRESULT3 >> 4);

        /* 清SEQ1完成标志 */
        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;

        /* IIR滤波 */
        LLC_Temp_V_    = (LLC_Temp_V    * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * LLC_Temp_V_;
        LLC_Temp_I_    = (LLC_Temp_I    * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * LLC_Temp_I_;
        LLC_Temp_Vin_  = (LLC_Temp_Vin  * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * LLC_Temp_Vin_;
        LLC_Temp_Iout_ = (LLC_Temp_Iout * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * LLC_Temp_Iout_;

        /* 工程量换算 */
        LLC_Values.Vout = LLC_Temp_V_;
        LLC_Values.IL   = LLC_Temp_I_    * LLC_ADC_INPUT_SCALE;
        LLC_Values.Vin  = LLC_Temp_Vin_  * LLC_ADC_INPUT_SCALE;
        LLC_Values.Iout = LLC_Temp_Iout_ * LLC_ADC_INPUT_SCALE;

        g_llc_adc_valid = 1;
    }

    /* 2. 启动“下一次”采样序列 */
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
    g_llc_adc_started = 1;
    return g_llc_adc_valid ? &LLC_Values : 0;
}

#pragma CODE_SECTION(IoutOffsetCal, ".TI.ramfunc")
/**
 * @brief 计算电流输出偏移
 * @param None
 * @return 如果校准完成则返回1，否则返回0
 */
char IoutOffsetCal(void)
{
    /* 电流偏移校准计数器 */
    static int Cnt=0;
    /* 偏移校准完成标志 */
    static  char Flag=0;
    /* 移动平均和 */
    static long IoutOffsetSum=0;

    /* 采样累加 */
    IoutOffsetSum += (AdcRegs.ADCRESULT3 >> 4);
    /* 偏移累加 */
    Cnt++;
    /* 累加256次 */
    if(Cnt>=LLC_ADC_OFFSET_SAMPLES)
    {
        /* 清除累加计时器 */
        Cnt=0;
        /* 计算256个样本的平均值作为偏移 */
        Flag=1;
        LLC_Values.Iout_Offset = (float)(IoutOffsetSum >> LLC_ADC_OFFSET_SHIFT);
        IoutOffsetSum = 0;
    }
    /* 返回偏移校准完成标志 */
    return Flag;
}
