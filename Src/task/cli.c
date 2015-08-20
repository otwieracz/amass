/* file: cli.c */

#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"

#include "error.h"
#include "hardware/uart.h"

void prvTaskCli(void *pvParameters);
void vTaskCliStart(void);

static const uint8_t* pcPromptMessage = (uint8_t*)"RTOS> ";
static const uint8_t* pcWelcomeMessage = (uint8_t*)"FreeRTOS.\r\nType \"help\" to view a list of registered commands.\r\n";

void vTaskCliStart(void)
{
    if(xTaskCreate(prvTaskCli, "prvTaskCli", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS)
    {
        vErrorFatal("prvTaskCli: Unable to start");
    }
}

void prvTaskCli(void *pvParameters)
{
    uint8_t cRxedChar;
    uint8_t cInputIndex = 0;
    BaseType_t xMoreDataToFollow;
    BaseType_t xReadState;

    /* The input and output buffers are declared static to keep them off the stack. */
    static uint8_t pcOutputString[USARTx_TXBUFFERSIZE];
    static uint8_t pcInputString[USARTx_RXBUFFERSIZE];

    /* Send a welcome message to the user knows they are connected. */
    xHardwareUartTx((uint8_t*)pcWelcomeMessage, strlen((const char*)pcWelcomeMessage));

    /* Send prompt */
    xHardwareUartTx((uint8_t*)pcPromptMessage, strlen((const char*)pcPromptMessage));

    while(1)
    {
        /* This implementation reads a single character at a time.  Wait in the
           Blocked state until a character is received. */

        xReadState = pdFAIL;
       
        while(xReadState != pdPASS) {
            xReadState = xHardwareUartRxChar(&cRxedChar);
        }
        xHardwareUartTxChar(cRxedChar);

        if(cRxedChar == '\n' || cRxedChar == '\r')
        {
            if(cInputIndex <= 0)
            {

                /* Send newline and prompt if just enter was pressed, without command */
                xHardwareUartTx((uint8_t*)"\r\n", 2);
                xHardwareUartTx((uint8_t*)pcPromptMessage, strlen((const char*)pcPromptMessage));
            } else
            /* Go ahead only if there is anything in command buffer
             * to handle '\r\n' situation */
            {
                /* A newline character was received, so the input command stirng is
                   complete and can be processed.  Transmit a line separator, just to 
                   make the output easier to read. */
                strncpy((char*)&pcOutputString, "\r\n", 3);
                xHardwareUartTx(pcOutputString, strlen((char*)pcOutputString));

                /* The command interpreter is called repeatedly until it returns 
                   pdFALSE.  See the "Implementing a command" documentation for an 
                   exaplanation of why this is. */
                do
                {
                    /* Send the command string to the command interpreter.  Any
                       output generated by the command interpreter will be placed in the 
                       pcOutputString buffer. */
                    xMoreDataToFollow = FreeRTOS_CLIProcessCommand
                        (     
                         (char*)pcInputString,       /* The command string.*/
                         (char*)pcOutputString,      /* The output buffer. */
                         USARTx_TXBUFFERSIZE  /* The size of the output buffer. */
                        );

                    /* Write the output generated by the command interpreter to the 
                       console. */
                    xHardwareUartTx(pcOutputString, strlen((char*)pcOutputString));

                } while(xMoreDataToFollow != pdFALSE);

                /* All the strings generated by the input command have been sent.
                   Processing of the command is complete.  Clear the input string ready 
                   to receive the next command. */
                cInputIndex = 0;
                memset(pcInputString, 0x00, USARTx_TXBUFFERSIZE);

                /* Send prompt */
                xHardwareUartTx((uint8_t*)pcPromptMessage, strlen((const char*)pcPromptMessage));
            }
        }
        else
        {
            /* The if() clause performs the processing after a newline character
               is received.  This else clause performs the processing if any other
               character is received. */
            if(cRxedChar == '\b')
            {
                /* Backspace was pressed.  Erase the last character in the input
                   buffer - if there are any. */
                if(cInputIndex > 0)
                {
                    cInputIndex--;
                    pcInputString[cInputIndex] = '\0';
                } else { 
                    /* If there is nothing to delete, recreate last character of prompt */
                    xHardwareUartTxChar(pcPromptMessage[strlen((const char*)pcPromptMessage)-1]);
                }
            }
            else
            {
                /* A character was entered.  It was not a new line, backspace
                   or carriage return, so it is accepted as part of the input and
                   placed into the input buffer.  When a \n is entered the complete
                   string will be passed to the command interpreter. */
                if(cInputIndex < USARTx_RXBUFFERSIZE)
                {
                    pcInputString[cInputIndex] = cRxedChar;
                    cInputIndex++;
                }
            }
        }
    }
}
