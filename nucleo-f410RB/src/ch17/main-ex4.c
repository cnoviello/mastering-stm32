/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <cmsis_os.h>
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <../system/include/retarget/retarget.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

void blinkThread(void const *argument);
void delayThread(void const *argument);

osSemaphoreId semid;

int main(void) {
  HAL_Init();

  Nucleo_BSP_Init();

  RetargetInit(&huart2);

  osThreadDef(blink, blinkThread, osPriorityNormal, 0, 100);
  osThreadCreate(osThread(blink), NULL);

  osThreadDef(delay, delayThread, osPriorityNormal, 0, 100);
  osThreadCreate(osThread(delay), NULL);

  osSemaphoreDef(sem);
  semid = osSemaphoreCreate(osSemaphore(sem), 1);
  osSemaphoreWait(semid, osWaitForever);

  osKernelStart();

  /* Infinite loop */
  while (1);
}

void blinkThread(void const *argument) {
  while(1) {
    osSemaphoreWait(semid, osWaitForever);
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  }
}

void delayThread(void const *argument) {
  while(1) {
    osDelay(500);
    osSemaphoreRelease(semid);
  }
}

#ifdef DEBUG

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ) {
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
