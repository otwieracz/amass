/* file: Src/task/gyro.c */

#include <stdio.h>
#include <string.h>

#include "stm32f3_discovery.h"
#include "stm32f3_discovery_gyroscope.h"
#include "stm32f3_discovery_accelerometer.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"

#include "error.h"
#include "task/gyro.h"
#include "hardware/hardware.h"

#define PRINT_TIMES 1000

void prvTaskGyro(void* pvParameters);

float fGyroX = 0;
float fGyroY = 0;
float fGyroZ = 0;

float fRotX = 0;
float fRotY = 0;
float fRotZ = 0;

/* CLI */
static BaseType_t prvTaskGyroRead(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
static const CLI_Command_Definition_t xTaskGyroReadCommand =
{
    "gyro_read",
    "gyro_read:\r\n Get current GYRO readings\r\n\r\n",
    prvTaskGyroRead,
    0
};
uint8_t ucPrintTimes = 0;

/* Task */
void vTaskGyroStart(void)
{
    /* Initialize all configured peripherals */
    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);
    BSP_LED_Init(LED5);
    BSP_LED_Init(LED6);
    BSP_LED_Init(LED7);
    BSP_LED_Init(LED8);
    BSP_LED_Init(LED9);
    BSP_LED_Init(LED10);

    if(BSP_GYRO_Init() != HAL_OK)
        vErrorFatal(__LINE__, __FILE__, "prvTaskGyro: GYRO init failed");
    BSP_GYRO_Reset();

    if(xTaskCreate(prvTaskGyro, "tGYRO", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS)
        vErrorFatal(__LINE__, __FILE__, "prvTaskGyro: Unable to start");

    FreeRTOS_CLIRegisterCommand(&xTaskGyroReadCommand);
}

void prvTaskGyro(void* pvParameters)
{
    float    fGyroReadBuffer[3];
    while(1)
    {
        /* Read GYRO */
        BSP_GYRO_GetXYZ(fGyroReadBuffer);
        fGyroX = fGyroReadBuffer[0];
        fGyroY = fGyroReadBuffer[1];
        fGyroZ = fGyroReadBuffer[2];

        /* Assume detected trend for 10ms */
        fRotX += (fGyroX/1000)*0.01;
        fRotY += (fGyroY/1000)*0.01;
        fRotZ += (fGyroZ/1000)*0.01;

        if(1 > fRotZ/45 && fRotZ/45 > -1)
            BSP_LED_On(LED3);
        else if(2 > fRotZ/45 && fRotZ/45 > 1)
            BSP_LED_On(LED5);
        else if(3 > fRotZ/45 && fRotZ/45 > 2)
            BSP_LED_On(LED7);
        else if(4 > fRotZ/45 && fRotZ/45 > 3)
            BSP_LED_On(LED9);
        else if(5 > fRotZ/45 && fRotZ/45 > 4)
            BSP_LED_On(LED10);
        else if(-1 > fRotZ/45 && fRotZ/45 > -2)
            BSP_LED_On(LED4);
        else if(-2 > fRotZ/45 && fRotZ/45 > -3)
            BSP_LED_On(LED6);
        else if(-3 > fRotZ/45 && fRotZ/45 > -4)
            BSP_LED_On(LED8);
        else if(-5 > fRotZ/45 && fRotZ/45 > -4)
            BSP_LED_On(LED10);

        vTaskDelay(10 / portTICK_PERIOD_MS);
        BSP_LED_Off(LED3);
        BSP_LED_Off(LED4);
        BSP_LED_Off(LED5);
        BSP_LED_Off(LED6);
        BSP_LED_Off(LED7);
        BSP_LED_Off(LED8);
        BSP_LED_Off(LED9);
        BSP_LED_Off(LED10);
    }
}

static BaseType_t prvTaskGyroRead(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
    memset(pcWriteBuffer, 0x00, xWriteBufferLen); 
    if(ucPrintTimes == 0)
        ucPrintTimes = PRINT_TIMES;

    if(ucPrintTimes > 0)
    {
        /* We assume that pcWriteBuffer is long enough */
        snprintf((char*)pcWriteBuffer,
                xWriteBufferLen,
                "%6d\t%6d\t%6d\r\n",
                (int16_t)fRotX,
                (int16_t)fRotY,
                (int16_t)fRotZ);

        HAL_Delay(40);
        ucPrintTimes--;
    }

    if(ucPrintTimes > 0)
    {
        /* Go for another lopp */
        return pdTRUE;
    } else
    {
        /* End command */
        return pdFALSE;
    }
}
