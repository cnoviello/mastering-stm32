/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>

/* Global macros */
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
    FLASH_EraseInitTypeDef eraseInfo;
    eraseInfo.Banks = FLASH_BANK_1;
    eraseInfo.NbSectors = 1;
    eraseInfo.Sector = FLASH_SECTOR_1;
    eraseInfo.TypeErase = FLASH_TYPEERASE_SECTORS;
    eraseInfo.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    uint32_t badBlocks = 0;

    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&eraseInfo, &badBlocks);
    while(1) {
          HAL_Delay(500);
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
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
      void (*reset_handler)(void) = (void*)APP_START_ADDRESS + 0x4;
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
