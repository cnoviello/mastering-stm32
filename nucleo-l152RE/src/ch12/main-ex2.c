/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
volatile uint8_t convCompleted = 0;

/* Private function prototypes -----------------------------------------------*/
static void MX_ADC1_Init(void);

int main(void) {
  char msg[20];
  uint16_t rawValues[3];
  float temp;

  HAL_Init();
  Nucleo_BSP_Init();

  /* Initialize all configured peripherals */
  MX_ADC1_Init();

  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)rawValues, 1);

  while(!convCompleted);

   for(uint8_t i = 0; i < 1; i++) {
    temp = ((float)rawValues[i]) / 4095 * 3300;
    temp = ((temp - 670.0) / 1.70) + 130;

    sprintf(msg, "rawValue %d: %hu\r\n", i, rawValues[i]);
    HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

    sprintf(msg, "Temperature %d: %f\r\n",i,  temp);
    HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
  }

  while (1);
}

/* ADC1 init function */
void MX_ADC1_Init(void) {
  ADC_ChannelConfTypeDef sConfig;

  /* Enable ADC peripheral */
  __HAL_RCC_ADC1_CLK_ENABLE();

  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
   */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
  hadc1.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  HAL_ADC_Init(&hadc1);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_384CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
  if(hadc->Instance==ADC1)  {
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    /* Peripheral DMA init*/
    hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_NORMAL;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_adc1);

    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  convCompleted = 1;
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
  asm("BKPT #0");
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
