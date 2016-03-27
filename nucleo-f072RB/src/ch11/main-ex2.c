/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim6;
DMA_HandleTypeDef hdma_tim6_up;


static void MX_DMA_Init(void);

int main(void) {
  uint8_t data[] = {0xFF, 0x0};

  HAL_Init();
  Nucleo_BSP_Init();
  MX_DMA_Init();

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 47999; //48MHz/48000 = 1000Hz
  htim6.Init.Period = 499;      //1000HZ / 500 = 2Hz = 0.5s
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  __TIM6_CLK_ENABLE();

  HAL_TIM_Base_Init(&htim6);
  HAL_TIM_Base_Start(&htim6);

  hdma_tim6_up.Instance = DMA1_Channel3;
  hdma_tim6_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim6_up.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim6_up.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim6_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tim6_up.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_tim6_up.Init.Mode = DMA_CIRCULAR;
  hdma_tim6_up.Init.Priority = DMA_PRIORITY_LOW;
  HAL_DMA_Init(&hdma_tim6_up);

  HAL_DMA_Start(&hdma_tim6_up, (uint32_t)data, (uint32_t)&GPIOA->ODR, 2);
  __HAL_TIM_ENABLE_DMA(&htim6, TIM_DMA_UPDATE);

  while (1);
}

/* Enable DMA controller clock */
void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __DMA1_CLK_ENABLE();
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
