/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include "TI_aes_128.h"

/* Global macros */

#define FALSE 0
#define TRUE 1

#define ACK 0x79
#define NACK 0x1F

#define CMD_ENTER_PROGRAM_MODE 0x7f
#define CMD_ERASE              0x43
#define CMD_GETID              0x02
#define CMD_WRITE              0x2b

#define SRAM_SIZE       96*1024     // STM32F401RE has 96 KB of RAM
#define SRAM_END        (SRAM_BASE + SRAM_SIZE)

#define APP_START_ADDRESS 0x08004000 /* In STM32F401RE this corresponds with the start
                                        address of Sector 1 */

/* Private variables ---------------------------------------------------------*/
uint8_t AES_KEY[] = { 0x4D, 0x61, 0x73, 0x74, 0x65, 0x72, 0x69, 0x6E, 0x67,
    0x20, 0x20, 0x53, 0x54, 0x4D, 0x33, 0x32 };
extern CRC_HandleTypeDef hcrc;
extern UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void _start(void);
void cmdErase(uint8_t *data);
void cmdGetID(uint8_t *data);
void cmdWrite(uint8_t *data);
int main(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void SysTick_Handler();

/* Minimal vector table */
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
    (uint32_t *) SRAM_END,    // initial stack pointer
    (uint32_t *) _start,  // _boot_init is the Reset_Handler
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (uint32_t *) SysTick_Handler };

// Begin address for the initialization values of the .data section.
// defined in linker script
extern uint32_t _sidata;
// Begin address for the .data section; defined in linker script
extern uint32_t _sdata;
// End address for the .data section; defined in linker script
extern uint32_t _edata;
// Begin address for the .bss section; defined in linker script
extern uint32_t _sbss;
// End address for the .bss section; defined in linker script
extern uint32_t _ebss;

inline void
__attribute__((always_inline))
__initialize_data(uint32_t* from, uint32_t* region_begin, uint32_t* region_end) {
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  uint32_t *p = region_begin;
  while (p < region_end)
    *p++ = *from++;
}

inline void
__attribute__((always_inline))
__initialize_bss(uint32_t* region_begin, uint32_t* region_end) {
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  uint32_t *p = region_begin;
  while (p < region_end)
    *p++ = 0;
}

void __attribute__ ((noreturn,weak))
_start(void) {
  __initialize_data(&_sidata, &_sdata, &_edata);
  __initialize_bss(&_sbss, &_ebss);
  main();

  for (;;)
    ;
}

int main(void) {
  uint32_t ticks = HAL_GetTick();
  uint8_t data[20];
  uint8_t inProgramMode = FALSE;
  UNUSED(AES_KEY);

  HAL_Init();
  MX_GPIO_Init();

  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
//  if(1) {
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    MX_CRC_Init();
    MX_USART2_UART_Init();

    ticks = HAL_GetTick();

    while (1) {
      if (HAL_GetTick() - ticks > 500) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        ticks = HAL_GetTick();
      }

      HAL_UART_Receive(&huart2, data, 1, 10);

      switch (data[0]) {
      case CMD_ENTER_PROGRAM_MODE:
        inProgramMode = TRUE;
        data[0] = ACK;
        HAL_UART_Transmit(&huart2, data, 1, HAL_MAX_DELAY);
        break;
      case CMD_GETID:
        if (inProgramMode) {
          cmdGetID(data);
        } else {
          data[0] = NACK;
          HAL_UART_Transmit(&huart2, data, 1, HAL_MAX_DELAY);
        }
        break;
      case CMD_ERASE:
        if (inProgramMode) {
          cmdErase(data);
        } else {
          data[0] = NACK;
          HAL_UART_Transmit(&huart2, data, 1, HAL_MAX_DELAY);
        }
        break;
      case CMD_WRITE:
        if (inProgramMode) {
          cmdWrite(data);
        } else {
          data[0] = NACK;
          HAL_UART_Transmit(&huart2, data, 1, HAL_MAX_DELAY);
        }
        break;
      };
    }
  }

  while (1) {
//    HAL_Delay(500);
//    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    if (*((uint32_t*) APP_START_ADDRESS) == 0xFFFFFFFF) {
      while (1) {
        HAL_Delay(30);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
      }
    } else {
      SCB->VTOR = APP_START_ADDRESS;
      __set_MSP(SRAM_END);
      RCC->CIR = 0x00000000;
      uint32_t JumpAddress = *(__IO uint32_t*) (0x08004000 + 4);
      void (*reset_handler)(void) = (void*)JumpAddress;
      reset_handler();
    }
  }
}

void cmdErase(uint8_t *data) {
  FLASH_EraseInitTypeDef eraseInfo;
  uint32_t badBlocks = 0;
  UNUSED(data);

  eraseInfo.Banks = FLASH_BANK_1;
  eraseInfo.NbSectors = 1;  //FLASH_SECTOR_TOTAL - 1;
  eraseInfo.Sector = FLASH_SECTOR_1;
  eraseInfo.TypeErase = FLASH_TYPEERASE_SECTORS;
  eraseInfo.VoltageRange = FLASH_VOLTAGE_RANGE_3;

  HAL_FLASH_Unlock();
  HAL_FLASHEx_Erase(&eraseInfo, &badBlocks);
  HAL_FLASH_Lock();

  data[0] = ACK;
  HAL_UART_Transmit(&huart2, (uint8_t *) data, 1, HAL_MAX_DELAY);

  for (uint8_t i = 0; i < 10; i++) {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_Delay(50);
  }
}

void cmdGetID(uint8_t *data) {
  UNUSED(data);
  uint16_t devID = (uint16_t) (DBGMCU->IDCODE & 0xFFF);

  HAL_UART_Transmit(&huart2, (uint8_t *) &devID, 2, HAL_MAX_DELAY);
}

void cmdWrite(uint8_t *data) {
  uint32_t addr = APP_START_ADDRESS;
  uint32_t msgContent = 0, crc = 0;
  uint32_t *data32 = (uint32_t*) data;
  uint8_t dn[20];

  data[0] = ACK;
  HAL_UART_Transmit(&huart2, (uint8_t *) data, 1, HAL_MAX_DELAY);

  HAL_FLASH_Unlock();
  while (1) {
    HAL_UART_Receive(&huart2, data, 20, HAL_MAX_DELAY);

    msgContent = 0;
    memcpy(&crc, data32 + 4, sizeof(uint32_t));
    memcpy(dn, data, sizeof(uint8_t)*20);

    if (crc == HAL_CRC_Calculate(&hcrc, data32, 4)) {
      aes_enc_dec((uint8_t*) data32, AES_KEY, 1);

      for (uint8_t i = 0; i < 4; i++)
        msgContent += data32[i];

      if (msgContent) {
        for (uint8_t i = 0; i < 4; i++) {
          HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data32[i]);
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
          addr += 4;
        }
      }
      data[0] = ACK;
    } else {
      data[0] = NACK;
    }

    HAL_UART_Transmit(&huart2, (uint8_t *) data, 1, HAL_MAX_DELAY);

    if (!msgContent)
      return;
  }
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
