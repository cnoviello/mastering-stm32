/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include <math.h>

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;

/* Private function prototypes -----------------------------------------------*/
static void MX_DAC_Init(void);

int main(void) {
  HAL_Init();
  Nucleo_BSP_Init();

  /* Initialize all configured peripherals */
  MX_DAC_Init();

  HAL_DAC_Init(&hdac);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

  while(1) {
    int i = 2000;
    while(i < 4000) {
      HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, i);
      HAL_Delay(1);
      i+=4;
    }

    while(i > 2000) {
      HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, i);
      HAL_Delay(1);
      i-=4;
    }
  }
}

/* DAC init function */
void MX_DAC_Init(void) {
  DAC_ChannelConfTypeDef sConfig;
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_DAC1_CLK_ENABLE();

  /* DAC Initialization  */
  hdac.Instance = DAC;
  HAL_DAC_Init(&hdac);

  /**DAC channel OUT1 config */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2);

  /* DAC GPIO Configuration
     PA5     ------> DAC_OUT1
  */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
