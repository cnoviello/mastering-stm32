/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <cmsis_os.h>
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <../system/include/retarget/retarget.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

void blinkFunc(void const *argument);

int main(void) {
  osTimerId stim1;

  HAL_Init();

  Nucleo_BSP_Init();

  RetargetInit(&huart2);

  osTimerDef(stim1, blinkFunc);
  stim1 = osTimerCreate(osTimer(stim1), osTimerPeriodic, NULL);
  osTimerStart(stim1, 500);

  osKernelStart();

  /* Infinite loop */
  while (1);
}

void blinkFunc(void const *argument) {
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
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
