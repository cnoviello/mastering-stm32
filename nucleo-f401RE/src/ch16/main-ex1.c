/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/

int main(void) {

  HAL_Init();
  Nucleo_BSP_Init();

  HAL_FLASH_Unlock();

  HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, 0x08008001, 0xAF);

  while(1);
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
