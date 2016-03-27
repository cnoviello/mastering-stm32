/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

void MX_TIM3_Init(void);

uint16_t computePulse(TIM_HandleTypeDef *htim, uint32_t chFrequency) {
  uint32_t timFrequency= HAL_RCC_GetPCLK1Freq() / (htim->Instance->PSC + 1);

  return (uint16_t)(timFrequency / chFrequency);
}

volatile uint16_t CH1_FREQ = 0;
volatile uint16_t CH2_FREQ = 0;

int main(void) {
  HAL_Init();

  Nucleo_BSP_Init();
  MX_TIM3_Init();

  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2);

  while (1);
}

/* TIM3 init function */
void MX_TIM3_Init(void) {
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 2;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_OC_Init(&htim3);

  CH1_FREQ = computePulse(&htim3, 50000);
  CH2_FREQ = computePulse(&htim3, 100000);

  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = CH1_FREQ;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = CH2_FREQ;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
  uint32_t pulse;

  /* TIMx_CH1 toggling with frequency = 50KHz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    pulse = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, (pulse + CH1_FREQ));
  }

  /* TIM2_CH2 toggling with frequency = 100KHz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    pulse = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, (pulse + CH2_FREQ));
  }
}

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim_base) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==TIM3) {
    __TIM3_CLK_ENABLE();

    /**TIM3 GPIO Configuration
    PA6     ------> TIM3_CH1
    PA7     ------> TIM3_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }
}

void TIM3_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim3);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if(htim->Instance == TIM3)
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
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
