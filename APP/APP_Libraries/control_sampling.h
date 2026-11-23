#ifndef APP_CONTROL_SAMPLING_H_
#define APP_CONTROL_SAMPLING_H_

#include "control_types.h"

/* 读取上一帧并触发下一帧。首帧有效前返回空指针，之后返回最近一次有效采样。 */
const struct _ADC_Values *LLC_ADC_ReadPrevious_And_StartNext(void);

/* 累加输出电流通道的原始计数，零偏校准完成后返回 1。 */
char IoutOffsetCal(void);

#endif /* APP_CONTROL_SAMPLING_H_ */
