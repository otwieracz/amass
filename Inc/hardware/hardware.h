/* hardware.c */

#ifndef _HARDWARE_HARDWARE_H
#define _HARDWARE_HARDWARE_H

#include "stm32f3xx_hal.h"     /* HAL */
#include "stm32f3_discovery.h" /* BSP */

/* Functions definitions */
void vHardwareSetup(void);
void vHardwareTimerSetup(
        TIM_HandleTypeDef* xTimerHandle, 
        TIM_TypeDef* usTimer,
        uint32_t ulPeriod,
        uint32_t ulFrequencyHz);
void vHardwareTimerPwmSetup(
        TIM_HandleTypeDef* xTimerHandle,
        TIM_OC_InitTypeDef* xTimerPwmConfig,
        uint8_t usChannel,
        uint8_t usPolarity,
        uint32_t ulPulseValue);

#endif /* _HARDWARE_HARDWARE_H */

