/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include "TI_aes_128.h"

/* Global macros */

#define ACK 0x79
#define NACK 0x1F

#define SRAM_SIZE       96*1024     // STM32F401RE has 96 KB of RAM
#define SRAM_END        (SRAM_BASE + SRAM_SIZE)

#define APP_START_ADDRESS 0x08004000 /* In STM32F401RE this corresponds with the start
                                        address of Sector 1 */

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void _start(void);
int main(void);
void SysTick_Handler();

/* Minimal vector table */
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
    (uint32_t *)SRAM_END,    // initial stack pointer
    (uint32_t *)_start,  // _boot_init is the Reset_Handler
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (uint32_t *)SysTick_Handler
};

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
__initialize_data (uint32_t* from, uint32_t* region_begin, uint32_t* region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  uint32_t *p = region_begin;
  while (p < region_end)
    *p++ = *from++;
}

inline void
__attribute__((always_inline))
__initialize_bss (uint32_t* region_begin, uint32_t* region_end) {
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  uint32_t *p = region_begin;
  while (p < region_end)
    *p++ = 0;
}

void __attribute__ ((noreturn,weak))
_start (void) {
  __initialize_data(&_sidata, &_sdata, &_edata);
  __initialize_bss(&_sbss, &_ebss);
  main();

  for(;;);
}

int main(void) {
  HAL_Init();
  Nucleo_BSP_Init();

  if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
    uint32_t ticks = HAL_GetTick();
    uint8_t cmd = 0;
    uint8_t data[16];
    const uint8_t key[] = {0x4D,0x61,0x73,0x74,0x65,0x72,0x69,0x6E,0x67,0x20,0x20,0x53,0x54,0x4D,0x33,0x32};

    HAL_UART_Transmit(&huart2, "ciao\r\n", strlen("ciao\r\n"), HAL_MAX_DELAY);

    while(1) {
      if(HAL_GetTick() - ticks > 500) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        ticks = HAL_GetTick();
      }
      HAL_UART_Receive(&huart2, data, 1, 10);
      if(data[0] == 0x7f) {
        data[0] = ACK;
        uint32_t addr = APP_START_ADDRESS;
        HAL_FLASH_Unlock();
        FLASH_EraseInitTypeDef eraseInfo;
        eraseInfo.Banks = FLASH_BANK_1;
        eraseInfo.NbSectors = 1;
        eraseInfo.Sector = FLASH_SECTOR_1;
        eraseInfo.TypeErase = FLASH_TYPEERASE_SECTORS;
        eraseInfo.VoltageRange = FLASH_VOLTAGE_RANGE_3;

        uint32_t badBlocks = 0;

        HAL_FLASHEx_Erase(&eraseInfo, &badBlocks);

        HAL_UART_Transmit(&huart2, data, 1, HAL_MAX_DELAY);
        while(1) {
          HAL_UART_Receive(&huart2, data, 16, HAL_MAX_DELAY);
          aes_enc_dec(data, key, 1);
          for(int i = 0; i < 16; i++) {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, data[i]);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            addr++;
          }
        }
      }
    }
  }

  while(1) {
//    HAL_Delay(500);
//    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    if(*((uint32_t*)APP_START_ADDRESS) == 0xFFFFFFFF)
    {
      while(1) {
        HAL_Delay(30);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
      }
    } else {
      SCB->VTOR = APP_START_ADDRESS;
      __set_MSP(SRAM_END);
      RCC->CIR = 0x00000000;
      uint32_t JumpAddress = *(__IO uint32_t*) (0x08004000 + 4);
      void (*reset_handler)(void) = JumpAddress;
      reset_handler();
    }
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
