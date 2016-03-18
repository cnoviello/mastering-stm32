/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim3;

/* Frequency Value */
float frequency = 0;
void MX_TIM3_Init(void);
void MX_TIM6_Init(void);
void MX_DMA_Init(void);

DMA_HandleTypeDef hdma_tim3_ch1_trig;
DMA_HandleTypeDef hdma_tim6_up;

uint8_t  odrVals[] = { 0x0, 0xFF };
uint16_t captures[2];
volatile uint8_t  captureDone = 0;

int main(void) {
  uint16_t diffCapture = 0;
  char msg[30];

  HAL_Init();

  Nucleo_BSP_Init();
  MX_DMA_Init();

  MX_TIM3_Init();
  MX_TIM6_Init();

  HAL_DMA_Start(&hdma_tim6_up, (uint32_t) odrVals, (uint32_t) &GPIOA->ODR, 2);
  __HAL_TIM_ENABLE_DMA(&htim6, TIM_DMA_UPDATE);
  HAL_TIM_Base_Start(&htim6);

  HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*) captures, 2);

  while (1) {
    if (captureDone != 0) {
      if (captures[1] >= captures[0])
        diffCapture = captures[1] - captures[0];
      else
        diffCapture = (htim3.Instance->ARR - captures[0]) + captures[1];

      frequency = HAL_RCC_GetPCLK1Freq() / (htim3.Instance->PSC + 1);
      frequency = (float) frequency / diffCapture;

      sprintf(msg, "Input frequency: %.3f\r\n", frequency);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
      while (1);
    }
  }
}

/* TIM3 init function */
void MX_TIM3_Init(void) {
  TIM_IC_InitTypeDef sConfigIC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_IC_Init(&htim3);

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1);
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim_ic) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if (htim_ic->Instance == TIM3) {
    /* Peripheral clock enable */
    __TIM3_CLK_ENABLE();

    /**TIM3 GPIO Configuration
     PA6     ------> TIM3_CH1
     */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral DMA init*/
    hdma_tim3_ch1_trig.Instance = DMA1_Channel4;
    hdma_tim3_ch1_trig.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim3_ch1_trig.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch1_trig.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch1_trig.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch1_trig.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim3_ch1_trig.Init.Mode = DMA_NORMAL;
    hdma_tim3_ch1_trig.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_tim3_ch1_trig);

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(htim_ic, hdma[TIM_DMA_ID_CC1], hdma_tim3_ch1_trig);
  }
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
    captureDone = 1;
  }
}

void MX_TIM6_Init(void) {
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 239; //48MHz/240 = 200KHz
  htim6.Init.Period = 1;      //200KHz/2  = 100KHz
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  HAL_TIM_Base_Init(&htim6);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
  if (htim_base->Instance == TIM6) {
    /* Peripheral clock enable */
    __TIM6_CLK_ENABLE();

    /* Peripheral DMA init*/
    hdma_tim6_up.Instance = DMA1_Channel3;
    hdma_tim6_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim6_up.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim6_up.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim6_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tim6_up.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_tim6_up.Init.Mode = DMA_CIRCULAR;
    hdma_tim6_up.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_tim6_up);

    __HAL_LINKDMA(htim_base, hdma[TIM_DMA_ID_UPDATE], hdma_tim6_up);
  }
}

void MX_DMA_Init(void) {
  /* DMA controller clock enable */
  __DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

}

void DMA1_Channel4_5_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_tim3_ch1_trig);
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
