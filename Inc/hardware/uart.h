/* file: uart.h */

#ifndef _HARDWARE_UART_H
#define _HARDWARE_UART_H

#include "error.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_uart.h"

/* Exported functions */
BaseType_t xHardwareUartInit(void);
BaseType_t xHardwareUartDeinit(void);
BaseType_t xHardwareUartTx(char* data, uint8_t data_length);
BaseType_t xHardwareUartRx(char* dest, uint8_t data_length);

/* Defines */

/* Transmission timeout (ms) */
#define USARTx_TIMEOUT                   -1

/* Definition for USARTx clock resources */
#define USARTx                           USART1
#define USARTx_BAUD                      9600
#define USARTx_CLK_ENABLE()              __USART1_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_4
#define USARTx_TX_GPIO_PORT              GPIOC
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_5
#define USARTx_RX_GPIO_PORT              GPIOC
#define USARTx_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler

UART_HandleTypeDef UartHandle;

#endif /* _HARDWARE_UART_H */

