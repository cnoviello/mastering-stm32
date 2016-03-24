/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <math.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

void MX_TIM3_Init(void);

DMA_HandleTypeDef hdma_tim3_ch1_trig;

#define PI    3.14159
#define ASR   1.8 //360 / 200 = 1.8

int main(void) {
  uint16_t IV[200];
  float angle;

  HAL_Init();

  Nucleo_BSP_Init();
  MX_TIM3_Init();

  for (uint8_t i = 0; i < 200; i++) {
    angle = ASR*(float)i;
    IV[i] = (uint16_t) rint(100 + 99*sinf(angle*(PI/180)));
  }

  HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)IV, 200);

  while (1);
}

/* TIM3 init function */
void MX_TIM3_Init(void) {
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 41;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 199;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   HAL_TIM_PWM_Init(&htim3);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

  hdma_tim3_ch1_trig.Instance = DMA1_Stream4;
  hdma_tim3_ch1_trig.Init.Channel = DMA_CHANNEL_5;
  hdma_tim3_ch1_trig.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim3_ch1_trig.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim3_ch1_trig.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim3_ch1_trig.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_tim3_ch1_trig.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_tim3_ch1_trig.Init.Mode = DMA_CIRCULAR;
  hdma_tim3_ch1_trig.Init.Priority = DMA_PRIORITY_LOW;
  hdma_tim3_ch1_trig.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  HAL_DMA_Init(&hdma_tim3_ch1_trig);

  /* Several peripheral DMA handle pointers point to the same DMA handle.
   Be aware that there is only one channel to perform all the requested DMAs. */
  __HAL_LINKDMA(&htim3, hdma[TIM_DMA_ID_CC1], hdma_tim3_ch1_trig);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_base) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if (htim_base->Instance == TIM3) {
    __TIM3_CLK_ENABLE();

    /**TIM3 GPIO Configuration
     PA6     ------> TIM3_CH1
     PA7     ------> TIM3_CH2
     */
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
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
