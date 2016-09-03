#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <stdlib.h>

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;
UART_HandleTypeDef huart2;

static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);

HAL_StatusTypeDef Write_To_24LCxx(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t len);
HAL_StatusTypeDef Read_From_24LCxx(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t len);

int main(void) {
  const char wmsg[] = "We love STM32!";
  char rmsg[20];

  HAL_Init();
  Nucleo_BSP_Init();

  MX_I2C1_Init();
  MX_I2C3_Init();

  HAL_I2C_EnableListen_IT(&hi2c3);


//  HAL_I2C_Master_Receive_IT(&hi2c3, 0x33, rmsg, 10);

  HAL_I2C_Master_Transmit(&hi2c1, 0x33, wmsg, 10, HAL_MAX_DELAY);

  if(strcmp(wmsg, rmsg) == 0) {
    while(1) {
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      HAL_Delay(100);
    }
  }

  while(1);
}

/* I2C1 init function */
static void MX_I2C1_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Peripheral clock enable */
  __HAL_RCC_I2C1_CLK_ENABLE();

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0xaa;
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
}

/* I2C3 init function */
static void MX_I2C3_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Peripheral clock enable */
  __HAL_RCC_I2C3_CLK_ENABLE();

  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0x33;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  /**I2C3 GPIO Configuration
  PA8     ------> I2C3_SCL
  PB4     ------> I2C3_SDA
  */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_I2C3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
  HAL_NVIC_SetPriority(I2C3_ER_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);

  HAL_I2C_Init(&hi2c3);
}

void I2C3_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C3_EV_IRQn 0 */

  /* USER CODE END I2C3_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c3);
  /* USER CODE BEGIN I2C3_EV_IRQn 1 */

  /* USER CODE END I2C3_EV_IRQn 1 */
}

/**
* @brief This function handles I2C3 error interrupt.
*/
void I2C3_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C3_ER_IRQn 0 */

  /* USER CODE END I2C3_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c3);
  /* USER CODE BEGIN I2C3_ER_IRQn 1 */

  /* USER CODE END I2C3_ER_IRQn 1 */
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  asm("BKPT #0");
}

HAL_StatusTypeDef Read_From_24LCxx(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t len) {
  HAL_StatusTypeDef returnValue;
  uint8_t addr[2];

  /* We compute the MSB and LSB parts of the memory address */
  addr[0] = (uint8_t) ((MemAddress & 0xFF00) >> 8);
  addr[1] = (uint8_t) (MemAddress & 0xFF);

  /* First we send the memory location address where start reading data */
  returnValue = HAL_I2C_Master_Transmit(hi2c, DevAddress, addr, 2, HAL_MAX_DELAY);
  if(returnValue != HAL_OK)
    return returnValue;

  /* Next we can retrieve the data from EEPROM */
  returnValue = HAL_I2C_Master_Receive(hi2c, DevAddress, pData, len, HAL_MAX_DELAY);

  return returnValue;
}

HAL_StatusTypeDef Write_To_24LCxx(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t len) {
  HAL_StatusTypeDef returnValue;
  uint8_t *data;

  /* First we allocate a temporary buffer to store the destination memory
   * address and the data to store */
  data = (uint8_t*)malloc(sizeof(uint8_t)*(len+2));

  /* We compute the MSB and LSB parts of the memory address */
  data[0] = (uint8_t) ((MemAddress & 0xFF00) >> 8);
  data[1] = (uint8_t) (MemAddress & 0xFF);

  /* And copy the content of the pData array in the temporary buffer */
  memcpy(data+2, pData, len);

  /* We are now ready to transfer the buffer over the I2C bus */
  returnValue = HAL_I2C_Master_Transmit(hi2c, DevAddress, data, len + 2, HAL_MAX_DELAY);
  if(returnValue != HAL_OK)
    return returnValue;

  free(data);

  /* We wait until the EEPROM effectively stores data in memory */
  while(HAL_I2C_Master_Transmit(hi2c, DevAddress, 0, 0, HAL_MAX_DELAY) != HAL_OK);

  return HAL_OK;
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
