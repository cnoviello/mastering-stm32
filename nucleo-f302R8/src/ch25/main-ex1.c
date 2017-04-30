#include "ioLibrary_Driver/Ethernet/socket.h"
#include <nucleo_hal_bsp.h>
#include "stm32f3xx_hal.h"
#include "ioLibrary_Driver/Ethernet/wizchip_conf.h"
#include "retarget/retarget-tcp.h"


SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;
UART_HandleTypeDef huart2;

void Error_Handler(void);
static void MX_SPI2_Init(void);

#define MAX_HTTPSOCK  6
#define DATA_BUF_SIZE 2048
uint8_t socknumlist[] = {2, 3, 4, 5, 6, 7};
volatile uint8_t RX_BUF[DATA_BUF_SIZE];
volatile uint8_t TX_BUF[DATA_BUF_SIZE];

void cs_sel() {
  HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_RESET); //CS LOW
}

void cs_desel() {
  HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET); //CS HIGH
}

uint8_t spi_rb(void) {
  uint8_t rbuf;
  HAL_SPI_Receive(&hspi2, &rbuf, 1, HAL_MAX_DELAY);
  return rbuf;
}

void spi_wb(uint8_t b) {
  HAL_SPI_Transmit(&hspi2, &b, 1, HAL_MAX_DELAY);
}

void spi_rb_burst(uint8_t *buf, uint16_t len) {
  HAL_SPI_Receive_DMA(&hspi2, buf, len);
  while(HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_BUSY_RX);
}

void spi_wb_burst(uint8_t *buf, uint16_t len) {
  HAL_SPI_Transmit_DMA(&hspi2, buf, len);
  while(HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_BUSY_TX);
}


void IO_LIBRARY_Init(void) {
  uint8_t bufSize[] = {2, 2, 2, 2};

  reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
  reg_wizchip_spi_cbfunc(spi_rb, spi_wb);
//  reg_wizchip_spiburst_cbfunc(spi_rb_burst, spi_wb_burst);

  wizchip_init(bufSize, bufSize);
  wiz_NetInfo netInfo = { .mac  = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef}, // Mac address
                          .ip   = {192, 168, 2, 165},         // IP address
                          .sn   = {255, 255, 255, 0},         // Subnet mask
                          .gw   = {192, 168, 2, 1}};          // Gateway address
  wizchip_setnetinfo(&netInfo);
  wizchip_setinterruptmask(IK_SOCK_0);
}

int main(void) {
  char buf[20];

  HAL_Init();
  Nucleo_BSP_Init();
  MX_SPI2_Init();

  IO_LIBRARY_Init();
  RetargetInit(0);

  while(1) {
    if(printf("Write your name: ")) {
      scanf("%s", buf);
      printf("\r\nYou wrote: %s\r\n", buf);
    }
    HAL_Delay(1000);
  }
}

/* SPI2 init function */
static void MX_SPI2_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Peripheral clock enable */
  __HAL_RCC_SPI2_CLK_ENABLE();

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Peripheral DMA init*/
  hdma_spi2_rx.Instance = DMA1_Channel4;
  hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_spi2_rx.Init.Mode = DMA_NORMAL;
  hdma_spi2_rx.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(&hspi2,hdmarx,hdma_spi2_rx);

  hdma_spi2_tx.Instance = DMA1_Channel5;
  hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_spi2_tx.Init.Mode = DMA_NORMAL;
  hdma_spi2_tx.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(&hspi2,hdmatx,hdma_spi2_tx);

  /**SPI2 GPIO Configuration
  PB13     ------> SPI2_SCK
  PB14     ------> SPI2_MISO
  PB15     ------> SPI2_MOSI
  */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

void Error_Handler(void) {
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
