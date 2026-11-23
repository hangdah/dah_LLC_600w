/**
 * @file mppt.h
 * @brief 最大功率点跟踪 (MPPT) 算法头文件
 * @date 2025
 * @author da
 *
 * 本文件定义了扰动观察法 (P&O) MPPT算法的数据结构和函数原型。
 * 支持基于电流和基于电压的跟踪模式。
 */

#ifndef APP_APP_LIBRARIES_MPPT_H_
#define APP_APP_LIBRARIES_MPPT_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

/** 基于电流的MPPT控制器结构 */
typedef struct {
    float  Ipv;         /* 光伏板电流 */
    float  Vpv;         /* 光伏板电压 */
    float  MaxI;        /* 最大电流限制 */
    float  MinI;        /* 最小电流限制 */
    float  Stepsize;    /* MPPT步长 */
    float  ImppOut;     /* 输出电流参考 */
    /* 内部变量 */
    float  Step;
    float  PpvOld;      /* 上一次的功率值 */
    float  IpvOld;      /* 上一次的电流值 */
    int mppt_first;     /* 首次运行标志 */
    int mppt_enable;    /* MPPT使能标志 */
} uinv_dcdc_mppt_t_I;

/** 基于电压的MPPT控制器结构 */
typedef struct {
    float  Ipv;         /* 光伏板电流 */
    float  Vpv;         /* 光伏板电压 */
    float  MaxV;        /* 最大电压限制 */
    float  MinV;        /* 最小电压限制 */
    float  Stepsize;    /* MPPT步长 */
    float  VmppOut;     /* 输出电压参考 */
    /* 内部变量 */
    float  Step;
    float  PpvOld;      /* 上一次的功率值 */
    float  VpvOld;      /* 上一次的电压值 */
    int mppt_first;     /* 首次运行标志 */
    int mppt_enable;    /* MPPT使能标志 */
} uinv_dcdc_mppt_t_V;

void uinv_dcdc_mppt_init_I(uinv_dcdc_mppt_t_I *v);
void uinv_dcdc_mppt_init_V(uinv_dcdc_mppt_t_V *v);
void uinv_dcdc_mppt_run_Impp(uinv_dcdc_mppt_t_I *v);
void uinv_dcdc_mppt_run_Vmpp(uinv_dcdc_mppt_t_V *v);




#endif /* APP_APP_LIBRARIES_MPPT_H_ */
