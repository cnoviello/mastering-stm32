/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim6;

int main(void) {
  HAL_Init();

  Nucleo_BSP_Init();

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 35999; //72MHz/36000 = 2000Hz
  htim6.Init.Period = 999;      //2000HZ / 1000 = 2Hz = 0.5s

  __TIM6_CLK_ENABLE();

  HAL_NVIC_SetPriority(TIM6_DAC1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_DAC1_IRQn);

  HAL_TIM_Base_Init(&htim6);
  HAL_TIM_Base_Start_IT(&htim6);

  while (1);
}

void TIM6_DAC1_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim6);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if(htim->Instance == TIM6)
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
