/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include "TI_aes_128.h"

/* Global macros */
#define ACK 0x79
#define NACK 0x1F
#define CMD_ERASE         0x43
#define CMD_GETID         0x02
#define CMD_WRITE         0x2b

#define APP_START_ADDRESS 0x08002C00 /* In STM32F030R8 this corresponds with the start
                                        address of Page 10 */

#define SRAM_SIZE         8*1024     // STM32F030R8 has 8 KB of RAM
#define SRAM_END          (SRAM_BASE + SRAM_SIZE)
#define FLASH_TOTAL_PAGES 64

#define ENABLE_BOOTLOADER_PROTECTION 0
#define PAGES_TO_PROTECT (OB_WRP_PAGES0TO3 | OB_WRP_PAGES4TO7)
/* Private variables ---------------------------------------------------------*/

/* The AES_KEY cannot be defined const, because the aes_enc_dec() function
 temporarily modifies its content */
uint8_t AES_KEY[] = { 0x4D, 0x61, 0x73, 0x74, 0x65, 0x72, 0x69, 0x6E, 0x67,
                      0x20, 0x20, 0x53, 0x54, 0x4D, 0x33, 0x32 };

extern CRC_HandleTypeDef hcrc;
extern UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void _start(void);
void CHECK_AND_SET_FLASH_PROTECTION(void);
void cmdErase(uint8_t *pucData);
void cmdGetID(uint8_t *pucData);
void cmdWrite(uint8_t *pucData);
int main(void);
void MX_CRC_Init(void);
void MX_GPIO_Deinit(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void SysTick_Handler();

/* Minimal vector table */
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
    (uint32_t *) SRAM_END, // initial stack pointer
    (uint32_t *) _start,   // _start is the Reset_Handler
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
  uint32_t ulTicks = 0;
  uint8_t ucUartBuffer[20];

  /* HAL_Init() sets SysTick timer so that it overflows every 1ms */
  HAL_Init();
  MX_GPIO_Init();

#if ENABLE_BOOTLOADER_PROTECTION
  /* Ensures that the first sector of flash is write-protected preventing that the
   bootloader is overwritten */
  CHECK_AND_SET_FLASH_PROTECTION();
#endif

  /* If USER_BUTTON is pressed */
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
    /* CRC and UART2 peripherals enabled */
    MX_CRC_Init();
    MX_USART2_UART_Init();

    ulTicks = HAL_GetTick();

    while (1) {
      /* Every 500ms the LD2 LED blinks, so that we can see the bootloader running. */
      if (HAL_GetTick() - ulTicks > 500) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        ulTicks = HAL_GetTick();
      }

      /* We check for new commands arriving on the UART2 */
      HAL_UART_Receive(&huart2, ucUartBuffer, 20, 10);
      switch (ucUartBuffer[0]) {
      case CMD_GETID:
        cmdGetID(ucUartBuffer);
        break;
      case CMD_ERASE:
        cmdErase(ucUartBuffer);
        break;
      case CMD_WRITE:
        cmdWrite(ucUartBuffer);
        break;
      };
    }
  } else {
    /* USER_BUTTON is not pressed. We first check if the first 4 bytes starting from
     APP_START_ADDRESS contain the MSP (end of SRAM). If not, the LD2 LED blinks quickly. */
    if (*((uint32_t*) APP_START_ADDRESS) != SRAM_END) {
      while (1) {
        HAL_Delay(30);
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      }
    } else {
      /* A valid program seems to exist in the second sector: we so prepare the MCU
       to start the main firmware */
      MX_GPIO_Deinit(); //Puts GPIOs in default state
      SysTick->CTRL = 0x0; //Disables SysTick timer and its related interrupt
      HAL_DeInit();

      RCC->CIR = 0x00000000; //Disable all interrupts related to clock

      uint32_t *pulSRAMBase = (uint32_t*)SRAM_BASE;
      uint32_t *pulFlashBase = (uint32_t*)APP_START_ADDRESS;
      uint16_t i = 0;

      do {
        if(pulFlashBase[i] == 0xAABBCCDD)
          break;
        pulSRAMBase[i] = pulFlashBase[i];
      } while(++i);

      __set_MSP(*((volatile uint32_t*) APP_START_ADDRESS)); //Set the MSP

      SYSCFG->CFGR1 |= 0x3; //__HAL_RCC_SYSCFG_CLK_ENABLE() already called from HAL_MspInit()

      /* We are now ready to jump to the main firmware */
      uint32_t JumpAddress = *((volatile uint32_t*) (APP_START_ADDRESS + 4));
      void (*reset_handler)(void) = (void*)JumpAddress;
      reset_handler(); //We start the execution from he Reset_Handler of the main firmware

      for (;;)
        ; //Never coming here
    }
  }
}

/* Performs a flash erase of a given number of sectors/pages.
 *
 * An erase command has the following structure:
 *
 * ----------------------------------------
 * | CMD_ID | # of sectors      |  CRC32  |
 * | 1 byte |     1 byte        | 4 bytes |
 * |--------|-------------------|---------|
 * |  0x43  | N or 0xFF for all |   CRC   |
 * ----------------------------------------
 */
void cmdErase(uint8_t *pucData) {
  FLASH_EraseInitTypeDef eraseInfo;
  uint32_t ulBadBlocks = 0, ulCrc = 0;
  uint32_t pulCmd[] = { pucData[0], pucData[1] };

  memcpy(&ulCrc, pucData + 2, sizeof(uint32_t));

  /* Checks if provided CRC is correct */
  if (ulCrc == HAL_CRC_Calculate(&hcrc, pulCmd, 2) &&
      (pucData[1] > 0 && (pucData[1] < FLASH_TOTAL_PAGES - 1 || pucData[1] == 0xFF))) {
    /* If data[1] contains 0xFF, it deletes all sectors; otherwise
     * the number of sectors specified. */
    eraseInfo.PageAddress = APP_START_ADDRESS;
    eraseInfo.NbPages = pucData[1] == 0xFF ? FLASH_TOTAL_PAGES - 10 : pucData[1];
    eraseInfo.TypeErase = FLASH_TYPEERASE_PAGES;

    HAL_FLASH_Unlock(); //Unlocks the flash memory
    HAL_FLASHEx_Erase(&eraseInfo, &ulBadBlocks); //Deletes given sectors */
    HAL_FLASH_Lock(); //Locks again the flash memory

    /* Sends an ACK */
    pucData[0] = ACK;
    HAL_UART_Transmit(&huart2, (uint8_t *) pucData, 1, HAL_MAX_DELAY);
  } else {
    /* The CRC is wrong: sends a NACK */
    pucData[0] = NACK;
    HAL_UART_Transmit(&huart2, pucData, 1, HAL_MAX_DELAY);
  }
}

/* Retrieve the STM32 MCU ID
 *
 * A GET_ID command has the following structure:
 *
 * --------------------
 * | CMD_ID |  CRC32  |
 * | 1 byte | 4 bytes |
 * |--------|---------|
 * |  0x02  |   CRC   |
 * --------------------
 */
void cmdGetID(uint8_t *pucData) {
  uint16_t usDevID;
  uint32_t ulCrc = 0;
  uint32_t ulCmd = pucData[0];

  memcpy(&ulCrc, pucData + 1, sizeof(uint32_t));

  /* Checks if provided CRC is correct */
  if (ulCrc == HAL_CRC_Calculate(&hcrc, &ulCmd, 1)) {
    usDevID = (uint16_t) (DBGMCU->IDCODE & 0xFFF); //Retrieves MCU ID from DEBUG interface

    /* Sends an ACK */
    pucData[0] = ACK;
    HAL_UART_Transmit(&huart2, pucData, 1, HAL_MAX_DELAY);

    /* Sends the MCU ID */
    HAL_UART_Transmit(&huart2, (uint8_t *) &usDevID, 2, HAL_MAX_DELAY);
  } else {
    /* The CRC is wrong: sends a NACK */
    pucData[0] = NACK;
    HAL_UART_Transmit(&huart2, pucData, 1, HAL_MAX_DELAY);
  }
}

/* Performs a write of 16 bytes starting from the specified address.
 *
 * A write command has the following structure:
 *
 * ----------------------------------------
 * | CMD_ID | starting address  |  CRC32  |
 * | 1 byte |     4 byte        | 4 bytes |
 * |--------|-------------------|---------|
 * |  0x2b  |    0x08004000     |   CRC   |
 * ----------------------------------------
 *
 * The second message has the following structure
 *
 * ------------------------------
 * |    data bytes    |  CRC32  |
 * |      16 bytes    | 4 bytes |
 * |------------------|---------|
 * | BBBBBBBBBBBBBBBB |   CRC   |
 * ------------------------------
 */
void cmdWrite(uint8_t *pucData) {
  uint32_t ulSaddr = 0, ulCrc = 0;

  memcpy(&ulSaddr, pucData + 1, sizeof(uint32_t));
  memcpy(&ulCrc, pucData + 5, sizeof(uint32_t));

  uint32_t pulData[5];
  for(int i = 0; i < 5; i++)
    pulData[i] = pucData[i];

  /* Checks if provided CRC is correct */
  if (ulCrc == HAL_CRC_Calculate(&hcrc, pulData, 5) && ulSaddr >= APP_START_ADDRESS) {
    /* Sends an ACK */
    pucData[0] = ACK;
    HAL_UART_Transmit(&huart2, (uint8_t *) pucData, 1, HAL_MAX_DELAY);

    /* Now retrieves given amount of bytes plus the CRC32 */
    if (HAL_UART_Receive(&huart2, pucData, 16 + 4, 200) == HAL_TIMEOUT)
      return;

    memcpy(&ulCrc, pucData + 16, sizeof(uint32_t));

    /* Checks if provided CRC is correct */
    if (ulCrc == HAL_CRC_Calculate(&hcrc, (uint32_t*) pucData, 4)) {
      HAL_FLASH_Unlock(); //Unlocks the flash memory

      /* Decode the sent bytes using AES-128 ECB */
      aes_enc_dec((uint8_t*) pucData, AES_KEY, 1);
      for (uint8_t i = 0; i < 4; i++) {
        /* Store each byte in flash memory starting from the specified address */
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ulSaddr, ((uint32_t*)pucData)[i]);
        ulSaddr += 4;
      }
      HAL_FLASH_Lock(); //Locks again the flash memory

      /* Sends an ACK */
      pucData[0] = ACK;
      HAL_UART_Transmit(&huart2, (uint8_t *) pucData, 1, HAL_MAX_DELAY);
    } else {
      goto sendnack;
    }
  } else {
    goto sendnack;
  }

sendnack:
  pucData[0] = NACK;
  HAL_UART_Transmit(&huart2, (uint8_t *) pucData, 1, HAL_MAX_DELAY);
}

void CHECK_AND_SET_FLASH_PROTECTION(void) {
  FLASH_OBProgramInitTypeDef obConfig;

  /* Retrieves current OB */
  HAL_FLASHEx_OBGetConfig(&obConfig);

  /* If the first sector is not protected */
  if ((obConfig.WRPPage & PAGES_TO_PROTECT) == PAGES_TO_PROTECT) {
    HAL_FLASH_Unlock(); //Unlocks flash
    HAL_FLASH_OB_Unlock(); //Unlocks OB
    obConfig.OptionType = OPTIONBYTE_WRP;
    obConfig.WRPState = OB_WRPSTATE_ENABLE; //Enables changing of WRP settings
    obConfig.WRPPage = PAGES_TO_PROTECT; //Enables WP on first pages
    HAL_FLASHEx_OBProgram(&obConfig); //Programs the OB
    HAL_FLASH_OB_Launch(); //Ensures that the new configuration is saved in flash
    HAL_FLASH_OB_Lock(); //Locks OB
    HAL_FLASH_Lock(); //Locks flash
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
