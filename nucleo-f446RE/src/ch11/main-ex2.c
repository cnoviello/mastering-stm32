/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
DMA_HandleTypeDef hdma_tim1_up;


static void MX_DMA_Init(void);

int main(void) {
  uint8_t data[] = {0xFF, 0x0};

  HAL_Init();
  Nucleo_BSP_Init();
  MX_DMA_Init();

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 47999; //84MHz/48000 = 1750Hz
  htim1.Init.Period = 874;      //1750HZ / 875 = 2Hz = 0.5s
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  __TIM1_CLK_ENABLE();

  HAL_TIM_Base_Init(&htim1);
  HAL_TIM_Base_Start(&htim1);

  hdma_tim1_up.Instance = DMA2_Stream5;
  hdma_tim1_up.Init.Channel = DMA_CHANNEL_6;
  hdma_tim1_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim1_up.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim1_up.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim1_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tim1_up.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_tim1_up.Init.Mode = DMA_CIRCULAR;
  hdma_tim1_up.Init.Priority = DMA_PRIORITY_LOW;
  hdma_tim1_up.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  HAL_DMA_Init(&hdma_tim1_up);

  HAL_DMA_Start(&hdma_tim1_up, (uint32_t)data, (uint32_t)&GPIOA->ODR, 2);
  __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);

  while (1);
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
