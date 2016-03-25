/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim8;
DMA_HandleTypeDef hdma_tim8_ch3_up;


static void MX_DMA_Init(void);

int main(void) {
  uint8_t data[] = {0xFF, 0x0};

  HAL_Init();
  Nucleo_BSP_Init();
  MX_DMA_Init();

  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 35999; //72MHz/36000 = 2000Hz
  htim8.Init.Period = 999;      //2000HZ / 1000 = 2Hz = 0.5s
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  __TIM8_CLK_ENABLE();

  HAL_TIM_Base_Init(&htim8);
  HAL_TIM_Base_Start(&htim8);

  hdma_tim8_ch3_up.Instance = DMA2_Channel1;
  hdma_tim8_ch3_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim8_ch3_up.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim8_ch3_up.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim8_ch3_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tim8_ch3_up.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_tim8_ch3_up.Init.Mode = DMA_CIRCULAR;
  hdma_tim8_ch3_up.Init.Priority = DMA_PRIORITY_LOW;
  HAL_DMA_Init(&hdma_tim8_ch3_up);

  HAL_DMA_Start(&hdma_tim8_ch3_up, (uint32_t)data, (uint32_t)&GPIOA->ODR, 2);
  __HAL_TIM_ENABLE_DMA(&htim8, TIM_DMA_UPDATE);

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
