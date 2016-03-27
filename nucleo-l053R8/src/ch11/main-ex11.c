/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <math.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim21, htim2;
UART_HandleTypeDef huart2;

void MX_TIM21_Init(void);
void MX_TIM2_Init(void);

char msg[40];
uint8_t dir = 0;
uint16_t cnt1 = 0, cnt2 = 0;
uint32_t tick = 0;
uint16_t diff = 0;
uint16_t tim1_ch1_pulse, tim1_ch2_pulse;
uint16_t speed = 0;

#define PULSES_PER_REVOLUTION 4

int main(void) {
  HAL_Init();

  Nucleo_BSP_Init();
  MX_TIM2_Init();
  MX_TIM21_Init();

  HAL_TIM_Encoder_Start(&htim21, TIM_CHANNEL_ALL);
  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2);

  cnt1 = __HAL_TIM_GET_COUNTER(&htim21);
  tick = HAL_GetTick();

  while (1) {
    if (HAL_GetTick() - tick > 1000L) {
      cnt2 = __HAL_TIM_GET_COUNTER(&htim21);
      if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim21)) {
        if (cnt2 < cnt1) /* Check for counter underflow */
          diff = cnt1 - cnt2;
        else
          diff = (65535 - cnt2) + cnt1;
      } else {
        if (cnt2 > cnt1) /* Check for counter overflow */
          diff = cnt2 - cnt1;
        else
          diff = (65535 - cnt1) + cnt2;
      }

      sprintf(msg, "Difference: %d\r\n", diff);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

      speed = ((diff / PULSES_PER_REVOLUTION) / 60);

      /* If the first three bits of SMCR register are set to 0x3
       * then the timer is set in X4 mode (TIM_ENCODERMODE_TI12)
       * and we need to divide the pulses counter by two, because
       * they include the pulses for both the channels */
      if ((TIM21->SMCR & 0x3) == 0x3)
        speed /= 2;

      sprintf(msg, "Speed: %d RPM\r\n", speed);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

      dir = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim21);
      sprintf(msg, "Direction: %d\r\n", dir);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

      tick = HAL_GetTick();
      cnt1 = __HAL_TIM_GET_COUNTER(&htim21);
    }

    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
      /* Invert rotation by swapping CH1 and CH2 CCR value */
      tim1_ch1_pulse = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_1);
      tim1_ch2_pulse = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_2);

      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, tim1_ch2_pulse);
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, tim1_ch1_pulse);
    }
  }
}

/* TIM2 init function */
void MX_TIM2_Init(void) {
  TIM_OC_InitTypeDef sConfigOC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 9;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  HAL_TIM_Base_Init(&htim2);

  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);

  sConfigOC.Pulse = 999; /* Phase B is shifted by 90° */
  HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
}

/* TIM21 init function */
void MX_TIM21_Init(void) {
  TIM_Encoder_InitTypeDef sEncoderConfig;

  htim21.Instance = TIM21;
  htim21.Init.Prescaler = 0;
  htim21.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim21.Init.Period = 65535;

  sEncoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;

  sEncoderConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sEncoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sEncoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sEncoderConfig.IC1Filter = 0;

  sEncoderConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sEncoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sEncoderConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sEncoderConfig.IC2Filter = 0;

  HAL_TIM_Encoder_Init(&htim21, &sEncoderConfig);
}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_base) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if (htim_base->Instance == TIM21) {
    __TIM21_CLK_ENABLE();

    /**TIM21 GPIO Configuration
    PB13     ------> TIM21_CH1
    PB14     ------> TIM21_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_TIM21;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if (htim_base->Instance == TIM2) {
    /* Peripheral clock enable */
    __TIM2_CLK_ENABLE();

    /**TIM2 GPIO Configuration
    PA0     ------> TIM2_CH1
    PA1     ------> TIM2_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /* USER CODE BEGIN TIM1_MspInit 1 */

    /* USER CODE END TIM1_MspInit 1 */
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
