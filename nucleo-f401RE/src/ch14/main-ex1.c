#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

static void MX_I2C1_Init(void);

int main(void) {
  uint8_t reg = 0, reg2;
  char msg[20];

  HAL_Init();
  Nucleo_BSP_Init();

  MX_I2C1_Init();

  while(1) {
    HAL_StatusTypeDef ret;

    HAL_I2C_Mem_Read(&hi2c1, (uint16_t)0xBE, (uint16_t)0xf, I2C_MEMADD_SIZE_8BIT, &reg, 1, HAL_MAX_DELAY);
    reg = 0x84;
    HAL_I2C_Mem_Write(&hi2c1, (uint16_t)0xBE, (uint16_t)0x20, I2C_MEMADD_SIZE_8BIT, &reg, 1, HAL_MAX_DELAY);
    reg = 0x1;
    HAL_I2C_Mem_Write(&hi2c1, (uint16_t)0xBE, (uint16_t)0x21, I2C_MEMADD_SIZE_8BIT, &reg, 1, HAL_MAX_DELAY);
    HAL_Delay(100);
    ret = HAL_I2C_Mem_Read(&hi2c1, (uint16_t)0xBE, (uint16_t)0x2A, I2C_MEMADD_SIZE_8BIT, &reg, 1, HAL_MAX_DELAY);
    ret = HAL_I2C_Mem_Read(&hi2c1, (uint16_t)0xBE, (uint16_t)0x2B, I2C_MEMADD_SIZE_8BIT, &reg2, 1, HAL_MAX_DELAY);
    if(ret != HAL_OK)
      asm("BKPT #0");
    HAL_I2C_Master_Transmit(&hi2c1, 0xBE, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, 0xBE, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Sequential_Transmit_IT(&hi2c1, 0xBE, &reg, 2, I2C_NEXT_FRAME);

    uint16_t temp = reg2 << 8 | reg;
    sprintf(msg, "Ricevuto: %d\r\n", temp);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    while(1);
  }
}

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
