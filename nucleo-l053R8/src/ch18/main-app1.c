/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
char msg[] = "This is the APP1\r\n";

/* Private function prototypes -----------------------------------------------*/

int main(void) {
  HAL_Init();
  Nucleo_BSP_Init();

  HAL_UART_Transmit_IT(&huart2, (uint8_t*)msg, strlen(msg));

  while (1) {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_Delay(200);
  }
}

void USART2_IRQHandler(void) {
  HAL_UART_IRQHandler(&huart2);
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
