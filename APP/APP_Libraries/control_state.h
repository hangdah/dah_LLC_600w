#ifndef APP_CONTROL_STATE_H_
#define APP_CONTROL_STATE_H_

#include "control_types.h"

/* 每次慢环调用推进一次 LLC 主状态机。 */
void LLC_State_M(volatile struct _Ctr_values *control);
/* 预留的 Burst 控制接口，当前慢环尚未调用。 */
void LLC_BurstCtl(const volatile struct _Ctr_values *control);

#endif /* APP_CONTROL_STATE_H_ */
