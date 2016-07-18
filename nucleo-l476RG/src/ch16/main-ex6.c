/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include <cmsis_os.h>
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

void blinkThread(void const *argument);

uint8_t suspendBlink = 0;
osThreadId blinkTID;
osMessageQId GPIOEvent = 0;

extern volatile uint8_t ucSleepModeCalled;

static volatile uint32_t sleepTime = 50;

int main(void) {
  HAL_Init();
  Nucleo_BSP_Init();

  osThreadDef(blink, blinkThread, osPriorityNormal, 0, 100);
  blinkTID = osThreadCreate(osThread(blink), NULL);

  osMessageQDef(GPIOQueue, 1, uint8_t);
  GPIOEvent = osMessageCreate(osMessageQ(GPIOQueue), NULL);

  HAL_SuspendTick();

  osKernelStart();

  /* Infinite loop */
  while (1);
}

void SystemClock_Config_8MHz(void) {

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
      | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);

  //HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == B1_Pin) {
    osMessagePut(GPIOEvent, 1, 0);
    if (suspendBlink)
      osThreadResume(blinkTID);
  }
}

void blinkThread(void const *argument) {
  UNUSED(argument);

  while (1) {
    if (osMessageGet(GPIOEvent, 0).status == osEventMessage) {
      ucSleepModeCalled = 0;
      if (suspendBlink) {
        suspendBlink = 0;
        sleepTime = 50;
      } else if (sleepTime == 50) {
        sleepTime = 500;
      } else if (sleepTime == 500) {
        suspendBlink = 1;
        osThreadSuspend(NULL);
      }
    }
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    osDelay(sleepTime);
  }
}

#if configUSE_TICKLESS_IDLE == 2

void preSLEEP(TickType_t xModifiableIdleTime) {
  UNUSED(xModifiableIdleTime);

  HAL_SuspendTick();
  __enable_irq();
  __disable_irq();
}

void postSLEEP(TickType_t xModifiableIdleTime) {
  UNUSED(xModifiableIdleTime);

  HAL_ResumeTick();
}

void preSTOP() {
  __HAL_RCC_GPIOA_CLK_DISABLE();
  HAL_SuspendTick();
  __enable_irq();
  __disable_irq();
//  SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
}

void postSTOP() {
  SystemClock_Config();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  HAL_ResumeTick();
}


#endif

#ifdef DEBUG

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName) {
  UNUSED(pxTask);
  UNUSED(pcTaskName);
  asm("BKPT #0");
}

#endif

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

