/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

void MX_TIM3_Init(void);

int main(void) {
  HAL_Init();

  Nucleo_BSP_Init();
  MX_TIM3_Init();

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

  /* Connect a LED to PA6 pin to see the fading effect */
  uint16_t dutyCycle = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);

  while(1) {
    while(dutyCycle < __HAL_TIM_GET_AUTORELOAD(&htim3)) {
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ++dutyCycle);
      HAL_Delay(1);
    }

    while(dutyCycle > 0) {
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, --dutyCycle);
      HAL_Delay(1);
    }
  }
}

/* TIM3 init function */
void MX_TIM3_Init(void) {
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 499;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  HAL_TIM_PWM_Init(&htim3);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_base) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==TIM3) {
    __TIM3_CLK_ENABLE();

    /**TIM3 GPIO Configuration
    PA6     ------> TIM3_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
