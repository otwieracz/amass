/* error.h */

#include "hardware.h"

/* 
 * Error handling function.
 *
 */

/* Signal fatal error by blinking all LEDs */
void vErrorFatalLoop(void)
{
    while(1)
    {
        /* Flash all LEDs */
        BSP_LED_Toggle(LED10);
        BSP_LED_Toggle(LED9);
        BSP_LED_Toggle(LED8);
        BSP_LED_Toggle(LED7);
        BSP_LED_Toggle(LED6);
        BSP_LED_Toggle(LED5);
        BSP_LED_Toggle(LED4);
        BSP_LED_Toggle(LED3);
        HAL_Delay(350);
    }
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
    vErrorFatalLoop();
}

#endif

