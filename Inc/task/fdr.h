/* file: task/fdr.h */

#ifndef _TASK_FDR_H
#define _TASK_FDR_H

#include "semphr.h"

#ifndef TASK_FDR_SAMPLING_RATE
/* Fdr sampling rate (times per second) */
#define TASK_FDR_SAMPLING_RATE 100
#endif /* TASK_FDR_SAMPLING_RATE */

/* Data struct */
typedef struct
{
    float x;
    float y;
    float z;
} FdrDataChunk;

/* Semaphores */
SemaphoreHandle_t xFdrDataSem;

/* Exported function */
void vTaskFdrStart(void);

#endif /* _TASK_FDR_H */
