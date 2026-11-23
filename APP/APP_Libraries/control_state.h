#ifndef APP_CONTROL_STATE_H_
#define APP_CONTROL_STATE_H_

#include "control_types.h"

void LLC_State_M(volatile struct _Ctr_values *control);
/* Available for integration. The slow tick does not currently call it. */
void LLC_BurstCtl(const volatile struct _Ctr_values *control);

#endif /* APP_CONTROL_STATE_H_ */
