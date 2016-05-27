#include "stm32l4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <mxconstants.h>
#include <string.h>

extern UART_HandleTypeDef huart2;

void SleepMode(void);
void StandbyMode(void);
void StopMode(void);
void MX_GPIO_Deinit(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);

int main(void) {
  char msg[20];

  HAL_Init();
  Nucleo_BSP_Init();

  /* Before we can access to every register of the PWR peripheral we must enable it */
  __HAL_RCC_PWR_CLK_ENABLE();

  while (1) {
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB)) {
      /* If standby flag set in PWR->CSR, then the reset is generated from
       * the exit of the standby mode */
      sprintf(msg, "RESET after STANDBY mode\r\n");
      HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
      /* We have to explicitly clear the flag */
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    }

    sprintf(msg, "MCU in run mode\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET) {
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      HAL_Delay(100);
    }

    HAL_Delay(200);

    sprintf(msg, "Entering in SLEEP mode\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    SleepMode();

    sprintf(msg, "Exiting from SLEEP mode\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET);
    HAL_Delay(200);

    sprintf(msg, "Entering in STOP mode\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    StopMode();

    sprintf(msg, "Exiting from STOP mode\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET);
    HAL_Delay(200);

    sprintf(msg, "Entering in STANDBY mode\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    StandbyMode();

    while(1); //Never arrives here, since MCU is reset when exiting from STANDBY
  }
}


void SleepMode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Disable all GPIOs to reduce power */
  MX_GPIO_Deinit();

  /* Configure User push-button as external interrupt generator */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  HAL_UART_DeInit(&huart2);

  /* Suspend Tick increment to prevent wakeup by Systick interrupt.
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  __HAL_RCC_PWR_CLK_ENABLE();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume Tick interrupt if disabled prior to sleep mode entry*/
  HAL_ResumeTick();

  /* Reinitialize GPIOs */
  MX_GPIO_Init();

  /* Reinitialize UART2 */
  MX_USART2_UART_Init();
}

void StopMode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Disable all GPIOs to reduce power */
  MX_GPIO_Deinit();

  /* Configure User push-button as external interrupt generator */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  HAL_UART_DeInit(&huart2);

  /* Suspend Tick increment to prevent wakeup by Systick interrupt.
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /* We enable again the PWR peripheral */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  SystemClock_Config();

  /* Resume Tick interrupt if disabled prior to sleep mode entry*/
  HAL_ResumeTick();

  /* Reinitialize GPIOs */
  MX_GPIO_Init();

  /* Reinitialize UART2 */
  MX_USART2_UART_Init();
}


void StandbyMode(void) {
  MX_GPIO_Deinit();

  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  /* Enable WKUP pin */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

  /* Enter STANDBY mode */
  HAL_PWR_EnterSTANDBYMode();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if(GPIO_Pin == B1_Pin) {
    while(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET);
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
