/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <math.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3, htim1;
UART_HandleTypeDef huart2;

void MX_TIM3_Init(void);
void MX_TIM1_Init(void);

char msg[40];
uint8_t dir = 0;
uint16_t cnt = 0;
uint32_t tick = 0;
uint16_t diff = 0;
uint16_t tim1_ch1_pulse, tim1_ch2_pulse;
uint16_t speed = 0;

#define PULSES_PER_REVOLUTION 4

int main(void) {
  HAL_Init();

  Nucleo_BSP_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();

  HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);

  cnt = __HAL_TIM_GET_COUNTER(&htim3);
  tick = HAL_GetTick();

  while(1) {
    if(HAL_GetTick() - tick > 1000L) {
      if(!__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3))
        diff = cnt - __HAL_TIM_GET_COUNTER(&htim3);
      else
        diff = __HAL_TIM_GET_COUNTER(&htim3) - cnt;

      sprintf(msg, "Difference: %d\r\n", diff);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

      uint16_t speed = ((diff/PULSES_PER_REVOLUTION)/60);
      sprintf(msg, "Speed: %d rpm\r\n", speed);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

      dir = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);
      sprintf(msg, "Direction: %d\r\n", dir);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

      tick = HAL_GetTick();
      cnt = __HAL_TIM_GET_COUNTER(&htim3);
    }

    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
      /* Invert rotation by swapping CH1 and CH2 CCR value */
      tim1_ch1_pulse = __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1);
      tim1_ch2_pulse = __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_2);

      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, tim1_ch2_pulse);
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, tim1_ch1_pulse);
    }
  }
}

/* TIM1 init function */
void MX_TIM1_Init(void) {
  TIM_OC_InitTypeDef sConfigOC;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim1);

  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);

  sConfigOC.Pulse = 999; /* Phase B is shifted by 90° */

  HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
}

/* TIM3 init function */
void MX_TIM3_Init(void) {
  TIM_Encoder_InitTypeDef sEncoderConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;

  sEncoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;

  sEncoderConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sEncoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sEncoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sEncoderConfig.IC1Filter = 0;

  sEncoderConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sEncoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sEncoderConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sEncoderConfig.IC2Filter = 0;

  HAL_TIM_Encoder_Init(&htim3, &sEncoderConfig);
}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_base) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if (htim_base->Instance == TIM3) {
    __TIM3_CLK_ENABLE()
    ;

    /**TIM3 GPIO Configuration
     PA6     ------> TIM3_CH1
     PA7     ------> TIM3_CH2
     */
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* Peripheral clock enable */
    __TIM1_CLK_ENABLE();

        /**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    PA9     ------> TIM1_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM3)
    asm("BKPT #0");
}

void TIM3_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim3);
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
