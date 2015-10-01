/* file: Src/task/fdr.c */

#include <stdio.h>
#include <string.h>

#include "stm32f3_discovery.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"

#include "error.h"
#include "task/fdr.h"
#include "task/gyro.h"
#include "task/accel.h"
#include "task/attind.h"
#include "task/extctrl.h"
#include "hardware/hardware.h"

void prvTaskFdr(void* pvParameters);

/* CLI hook function and command definition */
static BaseType_t prvTaskFdrDump(
        uint8_t *pcWriteBuffer,
        size_t xWriteBufferLen,
        const int8_t *pcCommandString);

static const CLI_Command_Definition_t xTaskFdrDumpCommand =
{
    "fdr-dump",
    "fdr-dump:\r\n Read FDR\r\n\r\n",
    prvTaskFdrDump,
    0
};

/* Exported function, to start FDR */
void vTaskFdrStart(void)
{
    /* Create task and register CLI command */
    if(xTaskCreate(prvTaskFdr, "FDR", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
        vErrorFatal(__LINE__, __FILE__, "prvTaskFdr: Unable to start");
    FreeRTOS_CLIRegisterCommand(&xTaskFdrDumpCommand);
}

void prvTaskFdr(void* pvParameters)
{
    while(1)
    {
        /* Delay specified period of time */
        vTaskDelay(1000 / TASK_FDR_SAMPLING_RATE / portTICK_PERIOD_MS);
    }
}

/* CLI command for reading fdr for PRINT_TIMES times */
static BaseType_t prvTaskFdrDump(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
    /* Zero buffer */
    memset(pcWriteBuffer, 0x00, xWriteBufferLen); 

    /* We assume that pcWriteBuffer is long enough, block on semaphore*/
    //xSemaphoreTake(xFdrDpsSem, 0);
    //snprintf((char*)pcWriteBuffer,
    //        xWriteBufferLen,
    //        "%6d\t%6d\t%6d\r\n",
    //        (int16_t)xFdrDps.x,
    //        (int16_t)xFdrDps.y,
    //        (int16_t)xFdrDps.z);
    //xSemaphoreGive(xFdrDpsSem);

    return pdFALSE;
}
