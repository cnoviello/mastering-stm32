/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <math.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

void MX_TIM2_Init(void);

 int main(void) {
  HAL_Init();

  Nucleo_BSP_Init();
  MX_TIM2_Init();

  HAL_TIM_OnePulse_Start(&htim2, TIM_CHANNEL_1);

  while (1);
}

/* TIM2 init function */
void MX_TIM2_Init(void) {
  TIM_OnePulse_InitTypeDef sConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 47;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  HAL_TIM_OnePulse_Init(&htim2, TIM_OPMODE_REPETITIVE);

  /* Configure the Channel 1 */
  sConfig.OCMode = TIM_OCMODE_TOGGLE;
  sConfig.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfig.Pulse = 19999;

  /* Configure the Channel 2 */
  sConfig.ICPolarity = TIM_ICPOLARITY_RISING;
  sConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfig.ICFilter = 0;

  HAL_TIM_OnePulse_ConfigChannel(&htim2, &sConfig, TIM_CHANNEL_1, TIM_CHANNEL_2);
}

void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef* htim_base) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if (htim_base->Instance == TIM2) {
    __TIM2_CLK_ENABLE();

    /**TIM2 GPIO Configuration
     PA0     ------> TIM2_CH1
     PA1     ------> TIM2_CH2
     */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
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
