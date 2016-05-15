/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <cmsis_os.h>
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

void blinkThread(void const *argument);
void threadsDumpThread(void const *argument);


int main(void) {
  HAL_Init();

  Nucleo_BSP_Init();

  osThreadDef(blink, blinkThread, osPriorityNormal, 0, 100);
  osThreadCreate(osThread(blink), NULL);

  osThreadDef(threadsDump, threadsDumpThread, osPriorityNormal, 0, 150);
  osThreadCreate(osThread(threadsDump), NULL);


  osKernelStart();

  /* Infinite loop */
  while (1);
}

void threadsDumpThread(void const *argument) {
 TaskStatus_t *pxTaskStatusArray = NULL;
 char *pcBuf = NULL;
 char *pcStatus;

  while(1) {
    if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET) {
      /* Allocate the message buffer. */
      pcBuf = pvPortMalloc(100 * sizeof(char));

      /* Allocate an array index for each task. */
      pxTaskStatusArray = pvPortMalloc( uxTaskGetNumberOfTasks() * sizeof( TaskStatus_t ) );

      if( pcBuf != NULL && pxTaskStatusArray != NULL ) {
        /* Generate the (binary) data. */
        uxTaskGetSystemState( pxTaskStatusArray, uxTaskGetNumberOfTasks(), NULL );

        sprintf(pcBuf, "         LIST OF RUNNING THREADS         \r\n-----------------------------------------\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t*)pcBuf, strlen(pcBuf), HAL_MAX_DELAY);

        for(uint16_t i = 0; i < uxTaskGetNumberOfTasks(); i++ )
        {
          sprintf(pcBuf, "Thread: %s\r\n", pxTaskStatusArray[i].pcTaskName);
          HAL_UART_Transmit(&huart2, (uint8_t*)pcBuf, strlen(pcBuf), HAL_MAX_DELAY);

          sprintf(pcBuf, "Thread ID: %lu\r\n", pxTaskStatusArray[i].xTaskNumber);
          HAL_UART_Transmit(&huart2, (uint8_t*)pcBuf, strlen(pcBuf), HAL_MAX_DELAY);

          switch (pxTaskStatusArray[i].eCurrentState) {
          case eReady:
            pcStatus = "READY";
            break;
          case eBlocked:
            pcStatus = "BLOCKED";
            break;
          case eSuspended:
            pcStatus = "SUSPENDED";
            break;
          case eDeleted:
            pcStatus = "DELETED";
            break;

          default: /* Should not get here, but it is included
                      to prevent static checking errors. */
            pcStatus = 0x00;
            break;
          }

          sprintf(pcBuf, "\tStatus: %s\r\n", pcStatus);
          HAL_UART_Transmit(&huart2, (uint8_t*)pcBuf, strlen(pcBuf), HAL_MAX_DELAY);

          sprintf(pcBuf, "\tStack watermark number: %d\r\n", pxTaskStatusArray[i].usStackHighWaterMark);
          HAL_UART_Transmit(&huart2, (uint8_t*)pcBuf, strlen(pcBuf), HAL_MAX_DELAY);

          sprintf(pcBuf, "\tPriority: %lu\r\n", pxTaskStatusArray[i].uxCurrentPriority);
          HAL_UART_Transmit(&huart2, (uint8_t*)pcBuf, strlen(pcBuf), HAL_MAX_DELAY);

          sprintf(pcBuf, "\tRun-time time: %lu\r\n", pxTaskStatusArray[i].ulRunTimeCounter);
          HAL_UART_Transmit(&huart2, (uint8_t*)pcBuf, strlen(pcBuf), HAL_MAX_DELAY);

          sprintf(pcBuf, "\tRun-time time in percentage: %lu\r\n", pxTaskStatusArray[i].ulRunTimeCounter/portGET_RUN_TIME_COUNTER_VALUE() / 100);
          HAL_UART_Transmit(&huart2, (uint8_t*)pcBuf, strlen(pcBuf), HAL_MAX_DELAY);
        }

        vPortFree(pcBuf);
        vPortFree(pxTaskStatusArray);
      }
    }
  }
  osThreadTerminate(NULL);
}

void blinkThread(void const *argument) {
  while(1) {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    osDelay(500);
  }
  osThreadTerminate(NULL);
}

#ifdef DEBUG

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ) {
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
