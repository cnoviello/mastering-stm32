/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
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
uint16_t cnt1 = 0, cnt2 = 0;
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

  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);

  cnt1 = __HAL_TIM_GET_COUNTER(&htim3);
  tick = HAL_GetTick();

  while (1) {
    if (HAL_GetTick() - tick > 1000L) {
      cnt2 = __HAL_TIM_GET_COUNTER(&htim3);
      if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3)) {
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
      if ((TIM3->SMCR & 0x3) == 0x3)
        speed /= 2;

      sprintf(msg, "Speed: %d RPM\r\n", speed);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

      dir = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);
      sprintf(msg, "Direction: %d\r\n", dir);
      HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

      tick = HAL_GetTick();
      cnt1 = __HAL_TIM_GET_COUNTER(&htim3);
    }

    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
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
  htim1.Init.Prescaler = 19;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  HAL_TIM_Base_Init(&htim1);

  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);

  sConfigOC.Pulse = 999; /* Phase B is shifted by 90° */
  HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
}

/* TIM3 init function */
void MX_TIM3_Init(void) {
  TIM_Encoder_InitTypeDef sEncoderConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
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
    __TIM3_CLK_ENABLE();

    /**TIM3 GPIO Configuration
     PA6     ------> TIM3_CH1
     PA7     ------> TIM3_CH2
     */
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
  GPIO_InitTypeDef GPIO_InitStruct;
  if (htim_base->Instance == TIM1) {
    /* Peripheral clock enable */
    __TIM1_CLK_ENABLE();

    /**TIM1 GPIO Configuration
     PA8     ------> TIM1_CH1
     PA9     ------> TIM1_CH2
     */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
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
