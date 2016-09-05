#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <stdlib.h>

ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;
volatile uint8_t transferRequested, transferDirection;

static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);

typedef enum {
  REGISTER,
} TransactionStatus;

#define TEMP_OUT_L_REGISTER 0x0
#define TEMP_OUT_H_REGISTER 0x1
#define WHO_AM_I_REGISTER 0xF
#define WHO_AM_I_VALUE 0xBC

int main(void) {
  uint16_t rawValue;
  uint32_t lastConversion;
  float ftemp;
  int8_t t_low,t_high;
  char msg[20];
  uint8_t buf[2];

  HAL_Init();
  Nucleo_BSP_Init();

  MX_I2C1_Init();

#ifdef SLAVE_BOARD

  MX_ADC1_Init();
  HAL_ADC_Start(&hadc1);
  HAL_I2C_EnableListen_IT(&hi2c1);

  while(1) {
    while(!transferRequested) {
      if(HAL_GetTick() - lastConversion > 1000L) {
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

        rawValue = HAL_ADC_GetValue(&hadc1);
        ftemp = ((float)rawValue) / 4095 * 3300;
        ftemp = ((ftemp - 760.0) / 2.5) + 25;

        t_low = ftemp;
        t_high = (ftemp - t_low)*100;

        sprintf(msg, "ftemp: %f\r\n", ftemp);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

        sprintf(msg, "t_low: %d - t_high: %d\r\n", t_low, t_high);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

        lastConversion = HAL_GetTick();
      }
    }

    transferRequested = 0;

    HAL_I2C_Slave_Sequential_Receive_IT(&hi2c1, buf, 1, I2C_FIRST_FRAME);

    while(!transferRequested);

    if(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_LISTEN) /* Provare facendo trasferire al master 1 byte */
      asm("BKPT #0");

    switch(buf[0]) {
    case WHO_AM_I_REGISTER:
      buf[0] = WHO_AM_I_VALUE;
      break;
    case TEMP_OUT_L_REGISTER:
      buf[0] = t_low;
      break;
    case TEMP_OUT_H_REGISTER:
      buf[0] = t_high;
      break;
    default:
      buf[0] = 0xFF;
    }
    HAL_I2C_Slave_Sequential_Transmit_IT(&hi2c1, buf, 1, I2C_LAST_FRAME);
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {
    }

    transferRequested = 0;
  }

#else //Master board
  HAL_StatusTypeDef ret;
  buf[0] = WHO_AM_I_REGISTER;
  ret = HAL_I2C_Master_Sequential_Transmit_IT(&hi2c1, 0x33, buf, 1, I2C_FIRST_FRAME);
  while (!transferRequested);
  transferRequested = 0;
  ret = HAL_I2C_Master_Sequential_Receive_IT(&hi2c1, 0x33, buf, 1, I2C_LAST_FRAME);
  while (!transferRequested);
  transferRequested = 0;

  sprintf(msg, "WHO AM I: %x\r\n", buf[0]);
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

#endif

  while(1);
}

#ifdef SLAVE_BOARD

/* ADC1 init function */
void MX_ADC1_Init(void) {
  ADC_ChannelConfTypeDef sConfig;

  /* Enable ADC peripheral */
  __HAL_RCC_ADC1_CLK_ENABLE();

  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
   */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  HAL_ADC_Init(&hadc1);

  /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
   */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

#endif

/* I2C1 init function */
static void MX_I2C1_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Peripheral clock enable */
  __HAL_RCC_I2C1_CLK_ENABLE();

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0x33;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_I2C_Init(&hi2c1);

  /* Peripheral interrupt init */
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
  HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}

void I2C1_EV_IRQHandler(void) {
  HAL_I2C_EV_IRQHandler(&hi2c1);
}

/**
* @brief This function handles I2C3 error interrupt.
*/
void I2C1_ER_IRQHandler(void) {
  HAL_I2C_ER_IRQHandler(&hi2c1);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  transferRequested = 1;
  transferDirection = TransferDirection;
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  transferRequested = 1;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  transferRequested = 1;
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  transferRequested = 1;
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
