/* file: Src/task/accel.c */

#include <stdio.h>
#include <string.h>

#include "stm32f3_discovery.h"
#include "stm32f3_discovery_accelerometer.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"

#include "error.h"
#include "task/accel.h"
#include "hardware/hardware.h"

#define PRINT_TIMES 255

void prvTaskAccel(void* pvParameters);

/* CLI hook function and command definition */
static BaseType_t prvTaskAccelDebug(
        uint8_t *pcWriteBuffer,
        size_t xWriteBufferLen, 
        const int8_t *pcCommandString
        );

static const CLI_Command_Definition_t xTaskAccelDebugCommand =
{
    "accel",
    "accel:\r\n Get current ACCEL readings\r\n\r\n",
    prvTaskAccelDebug,
    0
};
uint8_t ucAccelPrintTimes = 0;

/* Exported function to start ACCEL */
void vTaskAccelStart(void)
{
    /* Init ACCEL */
    if(BSP_ACCELERO_Init() != HAL_OK)
        vErrorFatal(__LINE__, __FILE__, "prvTaskAccel: ACCEL init failed");

    /* Create data container and semaphore */
    xAccelSem = xSemaphoreCreateBinary();
    xSemaphoreTake(xAccelSem, 0);
    xAccel.x = 0;
    xAccel.y = 0;
    xAccel.z = 0;
    xSemaphoreGive(xAccelSem);

    /* Create task and register CLI command */
    if(xTaskCreate(prvTaskAccel, "ACCEL", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
        vErrorFatal(__LINE__, __FILE__, "prvTaskAccel: Unable to start");
    FreeRTOS_CLIRegisterCommand(&xTaskAccelDebugCommand);
}

void prvTaskAccel(void* pvParameters)
{
    int16_t    sAccelDebugBuffer[3] = {0};

    while(1)
    {
        /* Read ACCEL */
        BSP_ACCELERO_GetXYZ(sAccelDebugBuffer);

        /* Store value */
        xSemaphoreTake(xAccelSem, 0);
        xAccel.x = sAccelDebugBuffer[0];
        xAccel.y = sAccelDebugBuffer[1];
        xAccel.z = sAccelDebugBuffer[2];
        xSemaphoreGive(xAccelSem);
        
        /* Delay specified period of time */
        vTaskDelay(1000 / TASK_ACCEL_SAMPLING_RATE / portTICK_PERIOD_MS);
    }
}

/* CLI command for reading gyro for PRINT_TIMES times */
static BaseType_t prvTaskAccelDebug(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
    /* Zero buffer */
    memset(pcWriteBuffer, 0x00, xWriteBufferLen); 

    if(ucAccelPrintTimes == 0)
        ucAccelPrintTimes = PRINT_TIMES;

    if(ucAccelPrintTimes > 0)
    {
        /* We assume that pcWriteBuffer is long enough, block on semaphore*/
        xSemaphoreTake(xAccelSem, 0);
        snprintf((char*)pcWriteBuffer,
                xWriteBufferLen,
                "%6d\t%6d\t%6d\r\n",
                (int16_t)xAccel.x,
                (int16_t)xAccel.y,
                (int16_t)xAccel.z);
        xSemaphoreGive(xAccelSem);
        HAL_Delay(40);
        ucAccelPrintTimes--;
    }

    if(ucAccelPrintTimes > 0)
    {
        /* Go for another loop */
        return pdTRUE;
    } else
    {
        /* End command */
        return pdFALSE;
    }
}
