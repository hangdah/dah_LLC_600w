/**
 * @file key.c
 * @brief 矩阵键盘扫描驱动
 * @date 2025
 * @author da
 *
 * 矩阵配置: 3x3矩阵
 * - 行 (输出): GPIO48, GPIO49, GPIO50
 * - 列 (输入): GPIO12, GPIO13, GPIO14
 */

#include "key.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

/**
 * @brief 初始化键盘GPIO
 * @param None
 * @return None
 */
void Key_Init(){
    EALLOW;

    /* 配置列引脚为带拉上的输入 */
    GpioCtrlRegs.GPAMUX1.bit.GPIO12=0;
    GpioCtrlRegs.GPADIR.bit.GPIO12=0;
    GpioCtrlRegs.GPAPUD.bit.GPIO12=0;

    GpioCtrlRegs.GPAMUX1.bit.GPIO13=0;
    GpioCtrlRegs.GPADIR.bit.GPIO13=0;
    GpioCtrlRegs.GPAPUD.bit.GPIO13=0;

    GpioCtrlRegs.GPAMUX1.bit.GPIO14=0;
    GpioCtrlRegs.GPADIR.bit.GPIO14=0;
    GpioCtrlRegs.GPAPUD.bit.GPIO14=0;

    /* 配置行引脚为输出 */
    GpioCtrlRegs.GPBMUX2.bit.GPIO48=0;
    GpioCtrlRegs.GPBDIR.bit.GPIO48=1;
    GpioCtrlRegs.GPBPUD.bit.GPIO48=0;

    GpioCtrlRegs.GPBMUX2.bit.GPIO49=0;
    GpioCtrlRegs.GPBDIR.bit.GPIO49=1;
    GpioCtrlRegs.GPBPUD.bit.GPIO49=0;

    GpioCtrlRegs.GPBMUX2.bit.GPIO50=0;
    GpioCtrlRegs.GPBDIR.bit.GPIO50=1;
    GpioCtrlRegs.GPBPUD.bit.GPIO50=0;

    EDIS;

    /* 初始将所有行置高 */
    GpioDataRegs.GPBSET.bit.GPIO48=1;
    GpioDataRegs.GPBSET.bit.GPIO49=1;
    GpioDataRegs.GPBSET.bit.GPIO50=1;
}

/**
 * @brief 扫描矩阵键盘
 * @param mode 0=单次触发, 1=连续
 * @return 按键代码 (1-9) 或 0 表示无按键按下
 */
char Key_Scan(char mode){

    static char keyl1=1;
    static char keyl2=1;
    static char keyl3=1;

    /* 第1列扫描 */
    KEY_L1_SETL;
    KEY_L2_SETH;
    KEY_L3_SETH;
    if(keyl1==1&&(KEY_H1==0||KEY_H2==0||KEY_H3==0)){
        DELAY_US(10000);
        keyl1=0;
        if(KEY_H1==0) return KEY1_PRESS;
        else if(KEY_H2==0) return KEY4_PRESS;
        else if(KEY_H3==0) return KEY7_PRESS;
    }
    else if(KEY_H1==1||KEY_H2==1||KEY_H3==1) keyl1=1;
    if(mode) keyl1=1;

    /* 第2列扫描 */
    KEY_L1_SETH;
    KEY_L2_SETL;
    KEY_L3_SETH;
    if(keyl2==1&&(KEY_H1==0||KEY_H2==0||KEY_H3==0)){
        DELAY_US(10000);
        keyl2=0;
        if(KEY_H1==0) return KEY2_PRESS;
        else if(KEY_H2==0) return KEY5_PRESS;
        else if(KEY_H3==0) return KEY8_PRESS;
    }
    else if(KEY_H1==1||KEY_H2==1||KEY_H3==1) keyl2=1;
    if(mode) keyl2=1;

    /* 第3列扫描 */
    KEY_L1_SETH;
    KEY_L2_SETH;
    KEY_L3_SETL;
    if(keyl3==1&&(KEY_H1==0||KEY_H2==0||KEY_H3==0)){
        DELAY_US(10000);
        keyl3=0;
        if(KEY_H1==0) return KEY3_PRESS;
        else if(KEY_H2==0) return KEY6_PRESS;
        else if(KEY_H3==0) return KEY9_PRESS;
    }
    else if(KEY_H1==1||KEY_H2==1||KEY_H3==1) keyl3=1;
    if(mode) keyl3=1;
    return KEY_UNPRESS;
}
