#ifndef APP_CONTROL_ALGORITHM_H_
#define APP_CONTROL_ALGORITHM_H_

#include "control_types.h"

void LLC_V_PI_Clear(void);
void LLC_V_PI_Update(float vout, volatile struct _Ctr_values *control);

#endif /* APP_CONTROL_ALGORITHM_H_ */
