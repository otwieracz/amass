/* file: Src/task/ext.c */

#include <stdio.h>
#include <string.h>

#include "stm32f3_discovery.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"

#include "error.h"
#include "hardware/hardware.h"
#include "task/extctrl.h"

#define PRINT_TIMES 255

TickType_t xPitchTick    = 0;
TickType_t xPitchTickTmp = 0;
TickType_t xRollTick     = 0;
TickType_t xRollTickTmp  = 0;
TickType_t xYawTick      = 0;
TickType_t xYawTickTmp   = 0;

void prvTaskExtCtrl(void* pvParameters);
void prvExtCtrlHandler(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI3_IRQHandler(void);

/* CLI hook function and command definition */
static BaseType_t prvTaskExtCtrlDebug(
        uint8_t *pcWriteBuffer,
        size_t xWriteBufferLen,
        const int8_t *pcCommandString);

static const CLI_Command_Definition_t xTaskExtCtrlDebugCommand =
{
    "extctrl",
    "extctrl:\r\n Get current EXTCTRL readings\r\n\r\n",
    prvTaskExtCtrlDebug,
    0
};
uint8_t ucExtCtrlPrintTimes = 0;

/* Exported function, to start ExtCtrl */
void vTaskExtCtrlStart(void)
{
    GPIO_InitTypeDef xGpioInitStruct;

    /* Enable GPIO E */
    __GPIOD_CLK_ENABLE();
    
    /* Configure GPIO E 0-2 as inputs with pulldown */
    xGpioInitStruct.Pin = (GPIO_PIN_0 |GPIO_PIN_1 |GPIO_PIN_3);
    xGpioInitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    xGpioInitStruct.Pull = GPIO_PULLDOWN;
    xGpioInitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOD, &xGpioInitStruct);

    /* Configure line interrupts */
    HAL_NVIC_SetPriority(EXTI0_IRQn, 10, 10);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    HAL_NVIC_SetPriority(EXTI1_IRQn, 10, 10);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    HAL_NVIC_SetPriority(EXTI3_IRQn, 10, 10);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    /* Create data container and semaphore */
    xExtCtrlSem = xSemaphoreCreateBinary();
    xSemaphoreTake(xExtCtrlSem, 0);
    xExtCtrl.pitch  = 0;
    xExtCtrl.roll   = 0;
    xExtCtrl.yaw    = 0;
    xSemaphoreGive(xExtCtrlSem);

    /* Create task and register CLI command */
    if(xTaskCreate(prvTaskExtCtrl, "EXTCTRL", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS)
        vErrorFatal(__LINE__, __FILE__, "prvTaskExtCtrl: Unable to start");
    FreeRTOS_CLIRegisterCommand(&xTaskExtCtrlDebugCommand);
}

void prvTaskExtCtrl(void* pvParameters)
{
    while(1)
    {
        /* Store value converted from mdps to dps */
        xSemaphoreTake(xExtCtrlSem, 0);
        taskDISABLE_INTERRUPTS();
        xExtCtrl.pitch = xPitchTick;
   //     xPitchTick = 0;
        xExtCtrl.roll  = xRollTick;
    //    xRollTick = 0;
        xExtCtrl.yaw   = xYawTick;
     //   xYawTick = 0;
        taskENABLE_INTERRUPTS();
        /* Critical section */
        xSemaphoreGive(xExtCtrlSem);

        /* Delay specified period of time */
        vTaskDelay(1000 / TASK_EXTCTRL_SAMPLING_RATE / portTICK_PERIOD_MS);
    }
}

void prvExtCtrlHandler(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_PinState xPinState = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

    switch(xPinState)
    {
        case GPIO_PIN_SET:
            /* Rising edge */
            if(GPIO_Pin == GPIO_PIN_0)
                xPitchTickTmp = xTaskGetTickCountFromISR();
            else if(GPIO_Pin == GPIO_PIN_1)
                xRollTickTmp = xTaskGetTickCountFromISR();
            else if(GPIO_Pin == GPIO_PIN_3)
                xYawTickTmp = xTaskGetTickCountFromISR();
            break;
        case GPIO_PIN_RESET:
            /* Falling edge */
            if(GPIO_Pin == GPIO_PIN_0)
                xPitchTick = xTaskGetTickCountFromISR() - xPitchTickTmp;
            else if(GPIO_Pin == GPIO_PIN_1)
                xRollTick = xTaskGetTickCountFromISR() - xRollTickTmp;
            else if(GPIO_Pin == GPIO_PIN_3)
                xYawTick = xTaskGetTickCountFromISR() - xYawTickTmp;
            break;
        default:
            break;
    }
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
}

void EXTI0_IRQHandler(void)
{
    prvExtCtrlHandler(GPIOD, GPIO_PIN_0);
}
void EXTI1_IRQHandler(void)
{
    prvExtCtrlHandler(GPIOD, GPIO_PIN_1);
}
void EXTI3_IRQHandler(void)
{
    prvExtCtrlHandler(GPIOD, GPIO_PIN_3);
}

/* CLI command for reading extctrl for PRINT_TIMES times */
static BaseType_t prvTaskExtCtrlDebug(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
    /* Zero buffer */
    memset(pcWriteBuffer, 0x00, xWriteBufferLen); 

    if(ucExtCtrlPrintTimes == 0)
        ucExtCtrlPrintTimes = PRINT_TIMES;

    if(ucExtCtrlPrintTimes > 0)
    {
        /* We assume that pcWriteBuffer is long enough, block on semaphore*/
        xSemaphoreTake(xExtCtrlSem, 0);
        snprintf((char*)pcWriteBuffer,
                xWriteBufferLen,
                "%6d\t%6d\t%6d\r\n",
                (int16_t)xExtCtrl.pitch*1000,
                (int16_t)xExtCtrl.roll*1000,
                (int16_t)xExtCtrl.yaw*1000);
        xSemaphoreGive(xExtCtrlSem);
        HAL_Delay(40);
        ucExtCtrlPrintTimes--;
    }

    if(ucExtCtrlPrintTimes > 0)
    {
        /* Go for another loop */
        return pdTRUE;
    } else
    {
        /* End command */
        return pdFALSE;
    }
}
