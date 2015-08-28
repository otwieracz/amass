/* file: task/attind.h */

#ifndef _TASK_ATTIND_H
#define _TASK_ATTIND_H

#include "FreeRTOS.h"
#include "semphr.h"

#ifndef TASK_ATTIND_SAMPLING_RATE
/* Attitude Indicator sampling rate (times per second) */
#define TASK_ATTIND_SAMPLING_RATE 100
#endif /* TASK_ATTIND_SAMPLING_RATE */

#ifndef TASK_ATTIND_GYRO_COEF
/* Gyro coefficient in Attitude Indicator calculations */
#define TASK_ATTIND_GYRO_COEF 0.95
#endif /* TASK_ATTIND_GYRO_COEF */

#ifndef TASK_ATTIND_ACC_COEF
/* Accelerometer coefficient in Attitude Indicator calculations */
#define TASK_ATTIND_ACC_COEF 0.05
#endif /* TASK_ATTIND_ACC_COEF */

/* Data struct */
typedef struct {
    float pitch;
    float roll;
    float yaw;
} AttindData;

/* Data containers */
AttindData xAttindData;

/* Semaphores */
SemaphoreHandle_t xAttindDataSem;

/* Exported function */
void vTaskAttindStart(void);

#endif /* _TASK_ATTIND_H */

