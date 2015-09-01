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
/* Select timer */
#define TIMX TIM1
#define __TIMX_CLK_ENABLE() (__TIM1_CLK_ENABLE());
/* TIM timer ticks for minimal and maximal control input */
#define EXTCTRL_TIM_COUNT_MIN 100.0f
#define EXTCTRL_TIM_COUNT_MAX 200.0f

TickType_t        xPitchTick    = 0;
TickType_t        xPitchTickTmp = 0;
TickType_t        xRollTick     = 0;
TickType_t        xRollTickTmp  = 0;
TickType_t        xYawTick      = 0;
TickType_t        xYawTickTmp   = 0;
TIM_HandleTypeDef xTimeBaseHandle;

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
    GPIO_InitTypeDef  xGpioInitStruct;

    /* Enable GPIO E */
    __GPIOD_CLK_ENABLE();
    
    /* Configure TIMX as a time source for external controls measurements 
     * Set prescaler to achieve clock equal to 100kHz (to measure time within
     * 0.01ms range) */
    __TIMX_CLK_ENABLE();
    vHardwareTimerSetup(&xTimeBaseHandle, TIMX, 1000000, 100000);

    /* Start timer */
    if(HAL_TIM_Base_Init(&xTimeBaseHandle) != HAL_OK)
    {
        vErrorFatal(__LINE__, __FILE__, "prvTaskExtCtrl: Unable to init TIM1");
    }
    if(HAL_TIM_Base_Start_IT(&xTimeBaseHandle) != HAL_OK)
    {
        vErrorFatal(__LINE__, __FILE__, "prvTaskExtCtrl: Unable to start TIM1");
    }
    
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
        xExtCtrl.pitch = (xPitchTick - EXTCTRL_TIM_COUNT_MIN) / (EXTCTRL_TIM_COUNT_MAX - EXTCTRL_TIM_COUNT_MIN) * 100;
        xExtCtrl.roll  = (xRollTick - EXTCTRL_TIM_COUNT_MIN) / (EXTCTRL_TIM_COUNT_MAX - EXTCTRL_TIM_COUNT_MIN) * 100;
        xExtCtrl.yaw   = (xYawTick - EXTCTRL_TIM_COUNT_MIN) / (EXTCTRL_TIM_COUNT_MAX - EXTCTRL_TIM_COUNT_MIN) * 100;
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
                xPitchTickTmp = __HAL_TIM_GetCounter(&xTimeBaseHandle);
            else if(GPIO_Pin == GPIO_PIN_1)
                xRollTickTmp = __HAL_TIM_GetCounter(&xTimeBaseHandle);
            else if(GPIO_Pin == GPIO_PIN_3)
                xYawTickTmp = __HAL_TIM_GetCounter(&xTimeBaseHandle);
            break;
        case GPIO_PIN_RESET:
            /* Falling edge */
            if(GPIO_Pin == GPIO_PIN_0)
                xPitchTick = __HAL_TIM_GetCounter(&xTimeBaseHandle) - xPitchTickTmp;
            else if(GPIO_Pin == GPIO_PIN_1)
                xRollTick = __HAL_TIM_GetCounter(&xTimeBaseHandle) - xRollTickTmp;
            else if(GPIO_Pin == GPIO_PIN_3)
                xYawTick = __HAL_TIM_GetCounter(&xTimeBaseHandle) - xYawTickTmp;
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
                (int16_t)xExtCtrl.pitch,
                (int16_t)xExtCtrl.roll,
                (int16_t)xExtCtrl.yaw);
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

