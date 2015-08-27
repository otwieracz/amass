/* file: uart.c */

#include <stdint.h>
#include <string.h>

#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_uart.h"

#include "error.h"
#include "hardware/uart.h"

void prvHardwareUartGpioInit(UART_HandleTypeDef *huart)
{  
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable GPIO TX/RX clock */
    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();

    /* Enable USARTx clock */
    USARTx_CLK_ENABLE(); 

    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;

    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;

    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
}

void prvHardwareUartGpioDeinit(UART_HandleTypeDef *huart)
{
    USARTx_FORCE_RESET();
    USARTx_RELEASE_RESET();

    /* Configure UART Tx as alternate function  */
    HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
    /* Configure UART Rx as alternate function  */
    HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
}

BaseType_t xHardwareUartInit(void)
{
    /* Configure the UART peripheral */
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART configured as follows:
       - Word Length = 8 Bits
       - Stop Bit = One Stop bit
       - Parity = None
       - BaudRate = 9600 baud
       - Hardware flow control disabled (RTS and CTS signals) */
    UartHandle.Instance        = USARTx;

    UartHandle.Init.BaudRate   = USARTx_BAUD;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;

    prvHardwareUartGpioInit(&UartHandle);

    if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        vErrorWarning(__LINE__, __FILE__, "UART init failed");
        return pdFAIL;
    }

    return pdPASS;
}

BaseType_t xHardwareUartDeinit(void)
{
    prvHardwareUartGpioDeinit(&UartHandle);
    return pdPASS;
}

BaseType_t xHardwareUartTx(char* data, uint8_t data_length)
{
    if(HAL_UART_Transmit(&UartHandle, (uint8_t*)data, data_length, USARTx_TIMEOUT) != HAL_OK)
    {
        vErrorWarning(__LINE__, __FILE__, "UART: transmit failed");
        return pdFAIL;
    }
    return pdPASS;
}

BaseType_t xHardwareUartRx(char* dest, uint8_t data_length)
{
    if(HAL_UART_Receive(&UartHandle, (uint8_t*)dest, data_length, USARTx_TIMEOUT) != HAL_OK)
    {
        vErrorWarning(__LINE__, __FILE__, "UART: recieve failed");
        return pdFAIL;
    }
    return pdPASS;
}

