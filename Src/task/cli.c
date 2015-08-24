/* file: cli.c */

#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"

#include "error.h"
#include "hardware/uart.h"

#include "task/cli.h"

void    vTaskCliStart(void);
void    prvTaskCli(void *pvParameters);
void    prvUartSendPrompt(void);
void    prvHandleBackspace(void);
uint8_t prvParseCommand(char* inputBuffer, char* outputBuffer, uint8_t inputBufferSize, uint8_t outputBufferSize);

static const char* pcPromptMessage = "RTOS> ";
static const char* pcWelcomeMessage = "FreeRTOS.\r\nType \"help\" to view a list of registered commands.\r\n";


void vTaskCliStart(void)
{
    if(xTaskCreate(prvTaskCli, "tCLI", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS)
    {
        vErrorFatal("prvTaskCli: Unable to start");
    }
}

void prvTaskCli(void *pvParameters)
{
    char        cRxedChar;
    uint8_t     cInputIndex = 0;
    BaseType_t  xReadState;

    /* The input and output buffers are declared static to keep them off the stack. */
    static char pcOutputString[TX_BUFFER_SIZE];
    static char pcInputString[RX_BUFFER_SIZE];

    /* Send a welcome message to the user knows they are connected. */
    xHardwareUartTx((char*)pcWelcomeMessage, strlen(pcWelcomeMessage));

    /* Send prompt */
    prvUartSendPrompt();

    while(1)
    {
        /* This implementation reads a single character at a time.  Wait in the
           Blocked state until a character is received. */
        xReadState = pdFAIL;
        while(xReadState != pdPASS) {
            xReadState = xHardwareUartRx(&cRxedChar, 1);
        }

        switch(cRxedChar)
        {
            /* User pushed RETURN */
            case '\n':
            case '\r':
                /* Send newline and prompt if just enter was pressed, without command
                 * entered */
                if(cInputIndex <= 0)
                {
                    xHardwareUartTx("\r\n", 2);
                    prvUartSendPrompt();
                } else
                    /* Go ahead only if there is anything in command buffer
                     * to handle '\r\n' situation */
                {
                    /* A newline character was received, so the input command stirng is
                       complete and can be processed.  Transmit a line separator, just to 
                       make the output easier to read. */
                    strncpy((char*)&pcOutputString, "\r\n", 3);
                    xHardwareUartTx(pcOutputString, strlen(pcOutputString));

                    /* Parse command and reset cInputIndex to 0 */
                    cInputIndex = prvParseCommand(
                            (char*)&pcInputString,
                            (char*)&pcOutputString,
                            RX_BUFFER_SIZE,
                            TX_BUFFER_SIZE);

                    /* Send prompt */
                    prvUartSendPrompt();
                }
                break;
            case '\b':
                /* Backspace was pressed.  Erase the last character in the input
                   buffer - if there are any. */
                if(cInputIndex > 0)
                {
                    cInputIndex--;
                    pcInputString[cInputIndex] = '\0';
                    prvHandleBackspace();
                }
                break;
            default:
                if(cRxedChar < 32 || cRxedChar > 126) {
                    /* Ignore non-printable characters */
                }
                else
                {
                    /* A character was entered.  It was not a new line, backspace
                       or carriage return, so it is accepted as part of the input and
                       placed into the input buffer.  When a \n is entered the complete
                       string will be passed to the command interpreter. */
                    if(cInputIndex < RX_BUFFER_SIZE)
                    {
                        pcInputString[cInputIndex] = cRxedChar;
                        cInputIndex++;
                    }
                    xHardwareUartTx(&cRxedChar, 1);
                }
                break;
        }
    }
}

/* Send prompt to UART */
void prvUartSendPrompt(void)
{
    xHardwareUartTx((char*)pcPromptMessage, strlen(pcPromptMessage));
}

/* Handle Backspace character */
void prvHandleBackspace(void)
{
    char buf[4];

    /* move cursor back, type ' ' and move cursor back again
     * to behave just as regular backspace */
    snprintf(buf, 4, "\b \b");
    xHardwareUartTx((char*)&buf, 4);
}

/* Parse entered command */
uint8_t prvParseCommand(char* inputBuffer, char* outputBuffer, uint8_t inputBufferSize, uint8_t outputBufferSize)
{
    BaseType_t  xMoreDataToFollow;
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
             inputBuffer,          /* The command string.*/
             outputBuffer,         /* The output buffer. */
             outputBufferSize      /* The size of the output buffer. */
            );

        /* Write the output generated by the command interpreter to the 
           console. */
        xHardwareUartTx(outputBuffer, strlen(outputBuffer));

    } while(xMoreDataToFollow != pdFALSE);

    /* All the strings generated by the input command have been sent.
       Processing of the command is complete.  Clear the input string ready 
       to receive the next command. */
    memset(inputBuffer, 0x00, inputBufferSize); 
    return 0;
}

