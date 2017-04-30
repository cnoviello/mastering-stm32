#include <nucleo_hal_bsp.h>
#include <string.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "ioLibrary_Driver/Ethernet/wizchip_conf.h"
#include "ioLibrary_Driver/Internet/DHCP/dhcp.h"
#include "ioLibrary_Driver/Internet/httpServer/httpParser.h"
#include "ioLibrary_Driver/Internet/httpServer/httpServer.h"
#include "diag/Trace.h"
#include "config.h"

#include <cmsis_os.h>
#include <task.h>

#if defined(OS_USE_SEMIHOSTING) && !defined(OS_BASE_FS_PATH)
#error "OS_BASE_FS_PATH macro not declared! You need to define it at project level"
#endif

#define MAX_HTTPSOCK  7
#define DHCP_SOCK 0
#define DATA_BUF_SIZE 2048

ADC_HandleTypeDef hadc1;
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_adc1;
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim2;

void Error_Handler(void);
void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM6_Init(void);
void SetupW5500Thread(void const *argument);
FRESULT scan_files (TCHAR* path);

uint8_t socknumlist[] = {0, 1, 2, 3, 4, 5, 6, 8};
uint8_t RX_BUF[DATA_BUF_SIZE];
uint8_t TX_BUF[DATA_BUF_SIZE];
uint16_t adcConv[100], _adcConv[200];
uint8_t convComplete;
osSemaphoreId adcSemID;

#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
FATFS diskHandle;
#endif

void cs_sel() {
  HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_RESET); //CS LOW
}

void cs_desel() {
  HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET); //CS HIGH
}

uint8_t spi_rb(void) {
  uint8_t rbuf;
  HAL_SPI_Receive(&hspi1, &rbuf, 1, HAL_MAX_DELAY);
  return rbuf;
}

void spi_wb(uint8_t b) {
  HAL_SPI_Transmit(&hspi1, &b, 1, HAL_MAX_DELAY);
}

void spi_rb_burst(uint8_t *buf, uint16_t len) {
  HAL_SPI_Receive_DMA(&hspi1, buf, len);
  while(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_RX);
}

void spi_wb_burst(uint8_t *buf, uint16_t len) {
  HAL_SPI_Transmit_DMA(&hspi1, buf, len);
  while(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
}

void IO_LIBRARY_Init(void) {
  uint8_t runApplication = 0, dhcpRetry = 0, phyLink = 0, bufSize[] = {2, 2, 2, 2, 2};
  wiz_NetInfo netInfo;

  reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
  reg_wizchip_spi_cbfunc(spi_rb, spi_wb);
  reg_wizchip_spiburst_cbfunc(spi_rb_burst, spi_wb_burst);
  reg_wizchip_cris_cbfunc(vPortEnterCritical, vPortExitCritical);

  wizchip_init(bufSize, bufSize);

  ReadNetCfgFromFile(&netInfo);

  /* Wait until the ETH cable is plugged in */
  do {
    ctlwizchip(CW_GET_PHYLINK, (void*) &phyLink);
    osDelay(10);
  } while(phyLink == PHY_LINK_OFF);

  if(netInfo.dhcp == NETINFO_DHCP) { /* DHCP Mode */
    DHCP_init(DHCP_SOCK, RX_BUF);

    while(!runApplication) {
      switch(DHCP_run()) {
      case DHCP_IP_LEASED:
      case DHCP_IP_ASSIGN:
      case DHCP_IP_CHANGED:
        getIPfromDHCP(netInfo.ip);
        getGWfromDHCP(netInfo.gw);
        getSNfromDHCP(netInfo.sn);
        getDNSfromDHCP(netInfo.dns);
        runApplication = 1;
        break;
      case DHCP_FAILED:
        dhcpRetry++;
        if(dhcpRetry > MAX_DHCP_RETRY)
        {
          netInfo.dhcp = NETINFO_STATIC;
          DHCP_stop();      // if restart, recall DHCP_init()
#ifdef _MAIN_DEBUG_
          printf(">> DHCP %d Failed\r\n", my_dhcp_retry);
          Net_Conf();
          Display_Net_Conf();   // print out static netinfo to serial
#endif
          dhcpRetry = 0;
          asm("BKPT #0");
        }
        break;
      default:
        break;
      }
    }
  }
  wizchip_setnetinfo(&netInfo);
}

int main(void) {
  HAL_Init();
  Nucleo_BSP_Init();

  osSemaphoreDef(adcSem);
  adcSemID = osSemaphoreCreate(osSemaphore(adcSem), 1);

  MX_ADC1_Init();
  MX_TIM6_Init();
  HAL_TIM_Base_Start(&htim2);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)_adcConv, 200);

  MX_SPI1_Init();

#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
  SD_SPI_Configure(SD_CS_GPIO_Port, SD_CS_Pin, &hspi1);
  MX_FATFS_Init();

  if(f_mount(&diskHandle, "0:", 1) != FR_OK) {
#ifdef DEBUG
    asm("BKPT #0");
#else
    while(1) {
      HAL_Delay(500);
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
#endif //#ifdef DEBUG
  }

#ifdef OS_USE_TRACE_ITM
  /* Prints the SD content over the ITM port */
  TCHAR buff[256];
  strcpy(buff, (char*)L"/");
  scan_files(buff);
#endif //#ifdef OS_USE_TRACE_ITM

#endif //#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)

  osThreadDef(w5500, SetupW5500Thread, osPriorityNormal, 0, 512);
  osThreadCreate(osThread(w5500), NULL);

  osKernelStart();
  /* Never coming here, but just in case... */
  while(1);
}

void SetupW5500Thread(void const *argument) {
  UNUSED(argument);

  /* Configure the W5500 module */
  IO_LIBRARY_Init();

  /* Configure the HTTP server */
  httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);
  reg_httpServer_cbfunc(NVIC_SystemReset, NULL);

  /* Start processing sockets */
  while(1) {
    for(uint8_t i = 0; i < MAX_HTTPSOCK; i++)
      httpServer_run(i);
    /* We just delay for 1ms so that other threads with the same
     * or lower priority can be executed */
    osDelay(1);
  }
}

/* ADC1 init function */
void MX_ADC1_Init(void) {
  ADC_ChannelConfTypeDef sConfig;
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable ADC peripheral */
  __HAL_RCC_ADC1_CLK_ENABLE();

  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
   */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  HAL_ADC_Init(&hadc1);

  /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
   */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  /**ADC1 GPIO Configuration
  PA0-WKUP     ------> ADC1_IN0
  */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Peripheral DMA init*/
  hdma_adc1.Instance = DMA2_Stream4;
  hdma_adc1.Init.Channel = DMA_CHANNEL_0;
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc1.Init.Mode = DMA_CIRCULAR;
  hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
  UNUSED(hadc);

  if(osSemaphoreWait(adcSemID, 0) == osOK) {
      memcpy(adcConv, _adcConv, sizeof(uint16_t)*100);
      osSemaphoreRelease(adcSemID);
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  UNUSED(hadc);

  if(osSemaphoreWait(adcSemID, 0) == osOK) {
      memcpy(adcConv, _adcConv+100, sizeof(uint16_t)*100);
      osSemaphoreRelease(adcSemID);
  }
}

/* SPI1 init function */
static void MX_SPI1_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Peripheral clock enable */
  __HAL_RCC_SPI1_CLK_ENABLE();

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Peripheral DMA init*/
  hdma_spi1_rx.Instance = DMA2_Stream0;
  hdma_spi1_rx.Init.Channel = DMA_CHANNEL_3;
  hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_spi1_rx.Init.Mode = DMA_NORMAL;
  hdma_spi1_rx.Init.Priority = DMA_PRIORITY_LOW;
  hdma_spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(&hspi1,hdmarx,hdma_spi1_rx);

  hdma_spi1_tx.Instance = DMA2_Stream3;
  hdma_spi1_tx.Init.Channel = DMA_CHANNEL_3;
  hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_spi1_tx.Init.Mode = DMA_NORMAL;
  hdma_spi1_tx.Init.Priority = DMA_PRIORITY_LOW;
  hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(&hspi1,hdmatx,hdma_spi1_tx);

  /**SPI1 GPIO Configuration
  PA5     ------> SPI1_SCK
  PA6     ------> SPI1_MISO
  PA7     ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_CS_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);

  /* Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : W5500_CS_Pin */
  GPIO_InitStruct.Pin = W5500_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(W5500_CS_GPIO_Port, &GPIO_InitStruct);

  /* Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET);
}

/* TIM2 init function */
static void MX_TIM6_Init(void) {
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_IC_InitTypeDef sConfigIC;
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_TIM6_CLK_ENABLE();

  htim2.Instance = TIM6;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4199;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Peripheral interrupt init */
  HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_IRQn);
}

#ifdef OS_USE_TRACE_ITM
FRESULT scan_files (TCHAR* path) {
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
    static TCHAR lfname[_MAX_LFN];
    TCHAR *fname;

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
#if _USE_LFN > 0
            fno.lfname = lfname;
            fno.lfsize = _MAX_LFN - 1;
#endif
            /* Read a directory item */
            res = f_readdir(&dir, &fno);
            /* Break on error or end of directory */
            if (res != FR_OK || fno.fname[0] == 0) break;
#if _USE_LFN > 0
            fname = *fno.lfname ? fno.lfname : fno.fname;
#endif
            if (fno.fattrib & AM_DIR) { /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fname);
                /* Scan directory recursively */
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else { /* It is a file. */
                trace_printf("%s/%s\n", path, fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}
#endif


void Error_Handler(void) {
#ifdef DEBUG
  asm("BKPT #0");
#else
  while(1) {
    HAL_Delay(250);
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  }
#endif
}


#ifdef DEBUG

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ) {
  UNUSED(pxTask);UNUSED(pcTaskName);

  Error_Handler();
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
