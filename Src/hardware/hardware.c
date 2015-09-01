/* hardware.c */

#include "stm32f3xx_hal.h"     /* HAL */
#include "stm32f3_discovery.h" /* BSP */

#include "hardware/hardware.h"
#include "hardware/uart.h"

void prvSystemClockConfig();

/* Main hardware-setup function, called from main.c */
void vHardwareSetup(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    prvSystemClockConfig();
    xHardwareUartInit();

}

/* System Clock Configuration by ST */
void prvSystemClockConfig(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

void vHardwareTimerSetup(
        TIM_HandleTypeDef* xTimerHandle,
        TIM_TypeDef* usTimer,
        uint32_t ulPeriod,
        uint32_t ulFrequencyHz)
{
    /* Configures Timer with Period == Frequency */

    uint32_t ulTimeBasePrescaler = (uint32_t)(SystemCoreClock / ulFrequencyHz) - 1;
    xTimerHandle->Instance           = usTimer;
    xTimerHandle->Init.Period        = ulPeriod;
    xTimerHandle->Init.Prescaler     = ulTimeBasePrescaler;
    xTimerHandle->Init.ClockDivision = 0;
    xTimerHandle->Init.CounterMode   = TIM_COUNTERMODE_UP;
    
}

void vHardwareTimerPwmSetup(
        TIM_HandleTypeDef* xTimerHandle,
        TIM_OC_InitTypeDef* xTimerPwmConfig,
        uint8_t usChannel,
        uint8_t usPolarity,
        uint32_t ulPulseValue)
{
    /* Configure PWM action */
    xTimerPwmConfig->OCMode     = TIM_OCMODE_PWM1;
    xTimerPwmConfig->OCPolarity = usPolarity;
    xTimerPwmConfig->OCFastMode = TIM_OCFAST_DISABLE;
    xTimerPwmConfig->Pulse      = ulPulseValue;


    /* Configure PWM */
    if(HAL_TIM_PWM_ConfigChannel(xTimerHandle, xTimerPwmConfig, usChannel) != HAL_OK)
    {
        /* Configuration Error */
        vErrorFatal(__LINE__, __FILE__, "Unable to init PWM");
    }

    /* Start PWM generation */
    if(HAL_TIM_PWM_Start(xTimerHandle, usChannel) != HAL_OK)
    {
        /* Configuration Error */
        vErrorFatal(__LINE__, __FILE__, "Unable to init PWM");
    }
}


