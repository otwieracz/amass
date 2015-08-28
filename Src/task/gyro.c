/* file: Src/task/gyro.c */

#include <stdio.h>
#include <string.h>

#include "stm32f3_discovery.h"
#include "stm32f3_discovery_gyroscope.h"
#include "stm32f3_discovery_accelerometer.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"

#include "error.h"
#include "task/gyro.h"
#include "hardware/hardware.h"

#define PRINT_TIMES 255

void prvTaskGyro(void* pvParameters);

/* CLI hook function and command definition */
static BaseType_t prvTaskGyroDebug(
        uint8_t *pcWriteBuffer,
        size_t xWriteBufferLen,
        const int8_t *pcCommandString);

static const CLI_Command_Definition_t xTaskGyroDebugCommand =
{
    "gyro",
    "gyro:\r\n Get current GYRO readings\r\n\r\n",
    prvTaskGyroDebug,
    0
};
uint8_t ucGyroPrintTimes = 0;

/* Exported function, to start Gyro */
void vTaskGyroStart(void)
{
    /* Init GYRO */
    if(BSP_GYRO_Init() != HAL_OK)
        vErrorFatal(__LINE__, __FILE__, "prvTaskGyro: GYRO init failed");
    BSP_GYRO_Reset();

    /* Create data container and semaphore */
    xGyroDpsSem = xSemaphoreCreateBinary();
    xSemaphoreTake(xGyroDpsSem, 0);
    xGyroDps.x  = 0;
    xGyroDps.y  = 0;
    xGyroDps.z  = 0;
    xSemaphoreGive(xGyroDpsSem);

    /* Create task and register CLI command */
    if(xTaskCreate(prvTaskGyro, "GYRO", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
        vErrorFatal(__LINE__, __FILE__, "prvTaskGyro: Unable to start");
    FreeRTOS_CLIRegisterCommand(&xTaskGyroDebugCommand);
}

void prvTaskGyro(void* pvParameters)
{
    float    fGyroDebugBuffer[3];

    while(1)
    {
        /* Read GYRO */
        BSP_GYRO_GetXYZ(fGyroDebugBuffer);

        /* Store value converted from mdps to dps */
        xSemaphoreTake(xGyroDpsSem, 0);
        xGyroDps.x = fGyroDebugBuffer[0]/1000;
        xGyroDps.y = fGyroDebugBuffer[1]/1000;
        xGyroDps.z = fGyroDebugBuffer[2]/1000;
        xSemaphoreGive(xGyroDpsSem);
        
        /* Delay specified period of time */
        vTaskDelay(1000 / TASK_GYRO_SAMPLING_RATE / portTICK_PERIOD_MS);
    }
}

/* CLI command for reading gyro for PRINT_TIMES times */
static BaseType_t prvTaskGyroDebug(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
    /* Zero buffer */
    memset(pcWriteBuffer, 0x00, xWriteBufferLen); 

    if(ucGyroPrintTimes == 0)
        ucGyroPrintTimes = PRINT_TIMES;

    if(ucGyroPrintTimes > 0)
    {
        /* We assume that pcWriteBuffer is long enough, block on semaphore*/
        xSemaphoreTake(xGyroDpsSem, 0);
        snprintf((char*)pcWriteBuffer,
                xWriteBufferLen,
                "%6d\t%6d\t%6d\r\n",
                (int16_t)xGyroDps.x,
                (int16_t)xGyroDps.y,
                (int16_t)xGyroDps.z);
        xSemaphoreGive(xGyroDpsSem);
        HAL_Delay(40);
        ucGyroPrintTimes--;
    }

    if(ucGyroPrintTimes > 0)
    {
        /* Go for another loop */
        return pdTRUE;
    } else
    {
        /* End command */
        return pdFALSE;
    }
}
