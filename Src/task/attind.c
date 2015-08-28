/* file: task/attind.c
 *
 * Attitude Indicator (AI, ATTIND). Combine data from
 * tasks GYRO and ACCEL to provide more stable output
 */

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"

#include "error.h"
#include "hardware/hardware.h"
#include "task/attind.h"
#include "task/accel.h"
#include "task/gyro.h"

#define ABS(a) (((a) < 0) ? -(a) : (a))
#define PI_FLOAT     3.14159265f
/* CLI times to print value */
#define PRINT_TIMES 255 
#define PI_FLOAT     3.14159265f
#define PIBY2_FLOAT  1.5707963f

void vTaskAttindStart(void);
void prvTaskAttind(void *pvParameters);
void prvComplementaryFilter(AccelData xAccel, GyroData xGyroDps, AttindData* xAttindData, float sampleTimeS);
float fast_atan2f(float y, float x);

/* CLI hook function and command definition */
static BaseType_t prvTaskAttindDebug(
        uint8_t *pcWriteBuffer,
        size_t xWriteBufferLen,
        const int8_t *pcCommandString);

static const CLI_Command_Definition_t xTaskAttindDebugCommand =
{
    "attind",
    "attind:\r\n Get current ATTITUDE INDICATOR readings\r\n\r\n",
    prvTaskAttindDebug,
    0
};
uint8_t ucAttindPrintTimes = 0;

/* Fast atan approximation
 * from http://www.lists.apple.com/archives/PerfOptimization-dev/2005/Jan/msg00051.html
 *
 * |error| < 0.005
 */
float fast_atan2f(float y, float x)
{
	float atan;
	float z = y/x;

	if(x == 0.0f)
	{
		if(y > 0.0f) return PIBY2_FLOAT;
		if(y == 0.0f) return 0.0f;
		return -PIBY2_FLOAT;
	}
	if (ABS(z) < 1.0f )
	{
		atan = z/(1.0f + 0.28f*z*z);
		if(x < 0.0f)
		{
			if (y < 0.0f) return atan - PI_FLOAT;
			return atan + PI_FLOAT;
		}
	}
	else
	{
		atan = PIBY2_FLOAT - z/(z*z + 0.28f);
		if (y < 0.0f) return atan - PI_FLOAT;
	}
	return atan;
}


/* Exported function, to start Attitude Indicator */
void vTaskAttindStart(void)
{
    /* Create semaphore and init data container */
    xAttindDataSem    = xSemaphoreCreateBinary();
    xSemaphoreTake(xAttindDataSem, 0);
    xAttindData.pitch = 0;
    xAttindData.roll  = 0;
    xAttindData.yaw   = 0;
    xSemaphoreGive(xAttindDataSem);

    /* Create task */
    if(xTaskCreate(prvTaskAttind, "ATTIND", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
    {
        vErrorFatal(__LINE__, __FILE__, "prvTaskAttind: Unable to start");
    }
    FreeRTOS_CLIRegisterCommand(&xTaskAttindDebugCommand);
}

/* Main task function */
void prvTaskAttind(void *pvParams)
{
    while(1)
    {
        xSemaphoreTake(xAccelSem, 0);
        xSemaphoreTake(xGyroDpsSem, 0);
        xSemaphoreTake(xAttindDataSem, 0);
        prvComplementaryFilter(xAccel, xGyroDps, &xAttindData, 1.0/TASK_ATTIND_SAMPLING_RATE);
        xSemaphoreGive(xAccelSem);
        xSemaphoreGive(xGyroDpsSem);
        xSemaphoreGive(xAttindDataSem);

        vTaskDelay(1000 / TASK_ATTIND_SAMPLING_RATE / portTICK_PERIOD_MS);

    }
}

/* Complementary filter, integrating gyro and accelerometer readings */
void prvComplementaryFilter(AccelData xAccel, GyroData xGyroDps, AttindData* xAttindData, float sampleTimeS)
{
    float fPitchAcc;
    float fRollAcc;

    xAttindData->roll  += xGyroDps.x * sampleTimeS;
    xAttindData->pitch -= xGyroDps.y * sampleTimeS;
    xAttindData->yaw   += xGyroDps.z * sampleTimeS;

    /* Discard accelerometer data if out of limits */

    fRollAcc  = fast_atan2f(xAccel.y, xAccel.z) * 180 / PI_FLOAT;
    fPitchAcc = fast_atan2f(xAccel.x, xAccel.z) * 180 / PI_FLOAT;

    xAttindData->roll  = xAttindData->roll * TASK_ATTIND_GYRO_COEF + fRollAcc * TASK_ATTIND_ACC_COEF;
    xAttindData->pitch = xAttindData->pitch * TASK_ATTIND_GYRO_COEF - fPitchAcc * TASK_ATTIND_ACC_COEF;
}

/* CLI command for reading gyro for PRINT_TIMES times */
static BaseType_t prvTaskAttindDebug(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
    /* Zero buffer */
    memset(pcWriteBuffer, 0x00, xWriteBufferLen); 

    if(ucAttindPrintTimes == 0)
        ucAttindPrintTimes = PRINT_TIMES;

    if(ucAttindPrintTimes > 0)
    {
        /* We assume that pcWriteBuffer is long enough, block on semaphore*/
        xSemaphoreTake(xAttindDataSem, 0);
        snprintf((char*)pcWriteBuffer,
                xWriteBufferLen,
                "%6ld\t%6ld\t%6ld\r\n",
                (int32_t)xAttindData.pitch,
                (int32_t)xAttindData.roll,
                (int32_t)xAttindData.yaw);
        xSemaphoreGive(xAttindDataSem);
        HAL_Delay(40);
        ucAttindPrintTimes--;
    }

    if(ucAttindPrintTimes > 0)
    {
        /* Go for another loop */
        return pdTRUE;
    } else
    {
        /* End command */
        return pdFALSE;
    }
}

