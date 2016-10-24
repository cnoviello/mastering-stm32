#include "stm32f1xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <stdlib.h>

ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

#ifdef SLAVE_BOARD
static void MX_ADC1_Init(void);
#endif
static void MX_I2C1_Init(void);

volatile uint8_t transferDirection, transferRequested;

#define TEMP_OUT_INT_REGISTER   0x0
#define TEMP_OUT_FRAC_REGISTER  0x1
#define WHO_AM_I_REGISTER       0xF
#define WHO_AM_I_VALUE          0xBC
#define TRANSFER_DIR_WRITE      0x1
#define TRANSFER_DIR_READ       0x0
#define I2C_SLAVE_ADDR          0x33

int main(void) {
  char uartBuf[20];
  uint8_t i2cBuf[2];
  float ftemp;
  int8_t t_frac, t_int;

  HAL_Init();
  Nucleo_BSP_Init();

  MX_I2C1_Init();

#ifdef SLAVE_BOARD
  uint16_t rawValue;
  uint32_t lastConversion;

  MX_ADC1_Init();
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_ADC_Start(&hadc1);

  while(1) {
    HAL_I2C_EnableListen_IT(&hi2c1);
    while(!transferRequested) {
      if(HAL_GetTick() - lastConversion > 1000L) {
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

        rawValue = HAL_ADC_GetValue(&hadc1);
        ftemp = ((float)rawValue) / 4095 * 3300;
        ftemp = ((ftemp - 1430.0) / 4.3) + 25;

        t_int = ftemp;
        t_frac = (ftemp - t_int)*100;

        sprintf(uartBuf, "Temperature: %f\r\n", ftemp);
        HAL_UART_Transmit(&huart2, (uint8_t*)uartBuf, strlen(uartBuf), HAL_MAX_DELAY);

        sprintf(uartBuf, "t_int: %d - t_frac: %d\r\n", t_frac, t_int);
        HAL_UART_Transmit(&huart2, (uint8_t*)uartBuf, strlen(uartBuf), HAL_MAX_DELAY);

        lastConversion = HAL_GetTick();
      }
    }

    transferRequested = 0;

    if(transferDirection == TRANSFER_DIR_WRITE) {
      /* Master is sending register address */
      HAL_I2C_Slave_Sequential_Receive_IT(&hi2c1, i2cBuf, 1, I2C_FIRST_FRAME);
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_LISTEN);

      switch(i2cBuf[0]) {
        case WHO_AM_I_REGISTER:
          i2cBuf[0] = WHO_AM_I_VALUE;
          break;
        case TEMP_OUT_INT_REGISTER:
          i2cBuf[0] = t_int;
          break;
        case TEMP_OUT_FRAC_REGISTER:
          i2cBuf[0] = t_frac;
          break;
        default:
          i2cBuf[0] = 0xFF;
      }

      HAL_I2C_Slave_Sequential_Transmit_IT(&hi2c1, i2cBuf, 1, I2C_LAST_FRAME);
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
    }
  }

#else //Master board
  i2cBuf[0] = WHO_AM_I_REGISTER;
  HAL_I2C_Master_Sequential_Transmit_IT(&hi2c1, I2C_SLAVE_ADDR, i2cBuf,
                                        1, I2C_FIRST_FRAME);
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

  HAL_I2C_Master_Sequential_Receive_IT(&hi2c1, I2C_SLAVE_ADDR, i2cBuf,
                                       1, I2C_LAST_FRAME);
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

  sprintf(uartBuf, "WHO AM I: %x\r\n", i2cBuf[0]);
  HAL_UART_Transmit(&huart2, (uint8_t*) uartBuf, strlen(uartBuf), HAL_MAX_DELAY);

  i2cBuf[0] = TEMP_OUT_INT_REGISTER;
  HAL_I2C_Master_Sequential_Transmit_IT(&hi2c1, I2C_SLAVE_ADDR, i2cBuf,
                                        1, I2C_FIRST_FRAME);
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

  HAL_I2C_Master_Sequential_Receive_IT(&hi2c1, I2C_SLAVE_ADDR, (uint8_t*)&t_int,
                                       1, I2C_LAST_FRAME);
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

  i2cBuf[0] = TEMP_OUT_FRAC_REGISTER;
  HAL_I2C_Master_Sequential_Transmit_IT(&hi2c1, I2C_SLAVE_ADDR, i2cBuf,
                                        1, I2C_FIRST_FRAME);
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

  HAL_I2C_Master_Sequential_Receive_IT(&hi2c1, I2C_SLAVE_ADDR, (uint8_t*)&t_frac,
                                       1, I2C_LAST_FRAME);
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

  ftemp = ((float)t_frac)/100.0;
  ftemp += (float)t_int;

  sprintf(uartBuf, "Temperature: %f\r\n", ftemp);
  HAL_UART_Transmit(&huart2, (uint8_t*) uartBuf, strlen(uartBuf), HAL_MAX_DELAY);

#endif

  while (1);
}

void I2C1_EV_IRQHandler(void) {
  HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler(void) {
  HAL_I2C_ER_IRQHandler(&hi2c1);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
  UNUSED(AddrMatchCode);

  if(hi2c->Instance == I2C1) {
    transferRequested = 1;
    transferDirection = TransferDirection;
  }
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
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadc1);

    /**Configure Regular Channel
    */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

#endif

/* I2C1 init function */
static void MX_I2C1_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  __HAL_AFIO_REMAP_I2C1_ENABLE();
  /* Peripheral clock enable */
  __HAL_RCC_I2C1_CLK_ENABLE();

  /* This is also required before configuring the I2C peripheral
   * in STM32F1xx devices */
  __HAL_RCC_I2C1_FORCE_RESET();
  __HAL_RCC_I2C1_RELEASE_RESET();

  HAL_I2C_Init(&hi2c1);
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
