/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim2;

/* Frequency Value */
float frequency = 0;
void MX_TIM2_Init(void);
void MX_TIM6_Init(void);
void MX_DMA_Init(void);

DMA_HandleTypeDef hdma_tim2_ch1;
DMA_HandleTypeDef hdma_tim6_up;

uint32_t captures[2];
volatile uint8_t  captureDone = 0;

int main(void) {
  uint32_t diffCapture = 0;
  char msg[30];

  HAL_Init();

  Nucleo_BSP_Init();
  MX_DMA_Init();

  MX_TIM2_Init();
  MX_TIM6_Init();

  HAL_TIM_Base_Start_IT(&htim6);

  HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*) captures, 2);

  while (1) {
    if (captureDone != 0) {
      if (captures[1] >= captures[0])
        diffCapture = captures[1] - captures[0];
      else
        diffCapture = (htim2.Instance->ARR - captures[0]) + captures[1];

      frequency = HAL_RCC_GetHCLKFreq() / (htim2.Instance->PSC + 1);
      frequency = (float) frequency / diffCapture;

      sprintf(msg, "Input frequency: %.3f\r\n", frequency);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
      while (1);
    }
  }
}

/* TIM2 init function */
void MX_TIM2_Init(void) {
  TIM_IC_InitTypeDef sConfigIC;
  TIM_ClockConfigTypeDef sClockSourceConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffffffff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_IC_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1);
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim_ic) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if (htim_ic->Instance == TIM2) {
    /* Peripheral clock enable */
    __TIM2_CLK_ENABLE();

    /**TIM2 GPIO Configuration
     PA0     ------> TIM2_CH1
     */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral DMA init*/
    hdma_tim2_ch1.Instance = DMA1_Channel5;
    hdma_tim2_ch1.Init.Request = DMA_REQUEST_4;
    hdma_tim2_ch1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim2_ch1.Init.Mode = DMA_NORMAL;
    hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_tim2_ch1);

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(htim_ic,hdma[TIM_DMA_ID_CC1],hdma_tim2_ch1);  }
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
    captureDone = 1;
  }
}

void MX_TIM6_Init(void) {
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 399; //80MHz/400 = 200KHz
  htim6.Init.Period = 1;      //200KHz/2  = 100KHz
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  HAL_TIM_Base_Init(&htim6);

  HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
  if (htim_base->Instance == TIM6) {
    /* Peripheral clock enable */
    __TIM6_CLK_ENABLE();
  }
}

void MX_DMA_Init(void) {
  /* DMA controller clock enable */
  __DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

void DMA1_Channel5_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_tim2_ch1);
}

void TIM6_DAC_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim6);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if(htim->Instance == TIM6)
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
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
