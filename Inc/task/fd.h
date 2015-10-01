/* file: Src/task/fd.h */

#ifndef _TASK_FD_H
#define _TASK_FD_H

#include "FreeRTOS.h"

#ifndef FD_EXTCTRL_DEAD_ZONE
#define FD_EXTCTRL_DEAD_ZONE 5
#endif /* FD_EXTCTRL_DEAD_ZONE */

/* Exported functions */
void vTaskFdStart(void);

#endif /* _TASK_FD_H */

