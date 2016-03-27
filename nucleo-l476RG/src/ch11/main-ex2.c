/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim5;
DMA_HandleTypeDef hdma_tim5_ch3_up;


static void MX_DMA_Init(void);

int main(void) {
  uint8_t data[] = {0xFF, 0x0};

  HAL_Init();
  Nucleo_BSP_Init();
  MX_DMA_Init();

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 39999; //80MHz/40000 = 2000Hz
  htim5.Init.Period = 999;      //2000HZ / 1000 = 2Hz = 0.5s
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  __TIM5_CLK_ENABLE();

  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Start(&htim5);

  hdma_tim5_ch3_up.Instance = DMA2_Channel2;
  hdma_tim5_ch3_up.Init.Request = DMA_REQUEST_5;
  hdma_tim5_ch3_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim5_ch3_up.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim5_ch3_up.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim5_ch3_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tim5_ch3_up.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_tim5_ch3_up.Init.Mode = DMA_CIRCULAR;
  hdma_tim5_ch3_up.Init.Priority = DMA_PRIORITY_LOW;
  HAL_DMA_Init(&hdma_tim5_ch3_up);

  HAL_DMA_Start(&hdma_tim5_ch3_up, (uint32_t)data, (uint32_t)&GPIOA->ODR, 2);
  __HAL_TIM_ENABLE_DMA(&htim5, TIM_DMA_UPDATE);

  while (1);
}

/* Enable DMA controller clock */
void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __DMA2_CLK_ENABLE();
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
