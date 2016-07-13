/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <nucleo_hal_bsp.h>
#include <string.h>
#include "TI_aes_128.h"

/* Global macros */
#define ACK 0x79
#define NACK 0x1F
#define CMD_ERASE         0x43
#define CMD_GETID         0x02
#define CMD_WRITE         0x2b

#define APP_START_ADDRESS 0x08004000 /* In STM32F401RE this corresponds with the start
                                        address of Sector 1 */

#define SRAM_SIZE         96*1024     // STM32F401RE has 96 KB of RAM
#define SRAM_END          (SRAM_BASE + SRAM_SIZE)

#define ENABLE_BOOTLOADER_PROTECTION 0
/* Private variables ---------------------------------------------------------*/
//The AES_KEY cannot be defined const, because the aes_enc_dec() function temporarily
//modifies its content
uint8_t AES_KEY[] = { 0x4D, 0x61, 0x73, 0x74, 0x65, 0x72, 0x69, 0x6E, 0x67,
    0x20, 0x20, 0x53, 0x54, 0x4D, 0x33, 0x32 };

extern CRC_HandleTypeDef hcrc;
extern UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void _start(void);
void CHECK_AND_SET_FLASH_PROTECTION(void);
void cmdErase(uint8_t *data);
void cmdGetID(uint8_t *data);
void cmdWrite(uint8_t *data);
int main(void);
void MX_CRC_Init(void);
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

  HAL_Init();
  MX_GPIO_Init();

  /* Ensures that the first sector of flash is write-protected preventing that the
   bootloader is overwritten */
#if ENABLE_BOOTLOADER_PROTECTION
  CHECK_AND_SET_FLASH_PROTECTION();
#endif

  /* If USER_BUTTON is pressed */
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
    /* We set so that the SysTick timer overflows ever 1ms */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    MX_CRC_Init(); /* CRC module enabled */
    MX_USART2_UART_Init();

    ticks = HAL_GetTick();

    while (1) {
      /* Every 500ms the LD2 LED blinks, so that we can check that the bootloader
       is running. */
      if (HAL_GetTick() - ticks > 500) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        ticks = HAL_GetTick();
      }

      /* We check for new commands arriving on the UART2 */
      HAL_UART_Receive(&huart2, data, 20, 10);
      switch (data[0]) {
      case CMD_GETID:
        cmdGetID(data);
        break;
      case CMD_ERASE:
        cmdErase(data);
        break;
      case CMD_WRITE:
        cmdWrite(data);
        break;
      };
    }
  } else {
    /* USER_BUTTON is not pressed. We first check if the second sector is erased. If so,
     the LD2 LED blinks quickly. */
    if (*((uint32_t*) APP_START_ADDRESS) == 0xFFFFFFFF) {
      while (1) {
        HAL_Delay(30);
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      }
    } else {
      /* A valid program seems to exist in the second sector: we so prepare the MCU to start
       the main firmware */
      SCB->VTOR = APP_START_ADDRESS; //We relocate vector table to the sector 1
      __set_MSP(SRAM_END); //Set the MSP to the end of SRAM
      RCC->CIR = 0x00000000; //Disable all interrupts related to clock
      uint32_t JumpAddress = *(__IO uint32_t*) (0x08004000 + 4);
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
void cmdErase(uint8_t *data) {
  FLASH_EraseInitTypeDef eraseInfo;
  uint32_t badBlocks = 0, crc = 0;
  uint32_t cmd[] = { data[0], data[1] };

  memcpy(&crc, data + 2, sizeof(uint32_t));

  /* Checks if provided CRC is correct */
  if (crc == HAL_CRC_Calculate(&hcrc, cmd, 2)) {
    /* If data[1] contains 0xFF, it deletes all sectors; otherwise
     * the number of sectors specified. */
    eraseInfo.Banks = FLASH_BANK_1;
    eraseInfo.NbSectors = data[1] == 0xFF ? FLASH_SECTOR_TOTAL - 1 : data[1];
    eraseInfo.Sector = FLASH_SECTOR_1;
    eraseInfo.TypeErase = FLASH_TYPEERASE_SECTORS;
    eraseInfo.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    HAL_FLASH_Unlock(); //Unlocks the flash memory
    HAL_FLASHEx_Erase(&eraseInfo, &badBlocks); //Deletes given sectors */
    HAL_FLASH_Lock(); //Locks again the flash memory

    /* Sends an ACK */
    data[0] = ACK;
    HAL_UART_Transmit(&huart2, (uint8_t *) data, 1, HAL_MAX_DELAY);
  } else {
    /* The CRC is wrong: sends a NACK */
    data[0] = NACK;
    HAL_UART_Transmit(&huart2, data, 1, HAL_MAX_DELAY);
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
void cmdGetID(uint8_t *data) {
  uint16_t devID;
  uint32_t crc = 0;
  uint32_t cmd = data[0];

  memcpy(&crc, data + 1, sizeof(uint32_t));

  /* Checks if provided CRC is correct */
  if (crc == HAL_CRC_Calculate(&hcrc, &cmd, 1)) {
    devID = (uint16_t) (DBGMCU->IDCODE & 0xFFF); //Retrieves MCU ID from DEBUG interface

    /* Sends an ACK */
    data[0] = ACK;
    HAL_UART_Transmit(&huart2, data, 1, HAL_MAX_DELAY);

    /* Sends the MCU ID */
    HAL_UART_Transmit(&huart2, (uint8_t *) &devID, 2, HAL_MAX_DELAY);
  } else {
    /* The CRC is wrong: sends a NACK */
    data[0] = NACK;
    HAL_UART_Transmit(&huart2, data, 1, HAL_MAX_DELAY);
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
void cmdWrite(uint8_t *data) {
  uint32_t saddr = 0, crc = 0;

  memcpy(&saddr, data + 1, sizeof(uint32_t));
  memcpy(&crc, data + 5, sizeof(uint32_t));

  uint32_t data32[5];
  for(int i = 0; i < 5; i++)
    data32[i] = data[i];

  /* Checks if provided CRC is correct */
  if (crc == HAL_CRC_Calculate(&hcrc, data32, 5)) {
    /* Sends an ACK */
    data[0] = ACK;
    HAL_UART_Transmit(&huart2, (uint8_t *) data, 1, HAL_MAX_DELAY);

    /* Now retrieves given amount of bytes plus the CRC32 */
    if (HAL_UART_Receive(&huart2, data, 16 + 4, 200) == HAL_TIMEOUT)
      return;

    memcpy(&crc, data + 16, sizeof(uint32_t));

    /* Checks if provided CRC is correct */
    if (crc == HAL_CRC_Calculate(&hcrc, (uint32_t*) data, 4)) {
      HAL_FLASH_Unlock(); //Unlocks the flash memory

      /* Decode the sent bytes using AES-128 ECB */
      aes_enc_dec((uint8_t*) data, AES_KEY, 1);
      for (uint8_t i = 0; i < 16; i++) {
        /* Store each byte in flash memory starting from the specified address */
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, saddr, data[i]);
        saddr += 1;
      }
      HAL_FLASH_Lock(); //Locks again the flash memory

      /* Sends an ACK */
      data[0] = ACK;
      HAL_UART_Transmit(&huart2, (uint8_t *) data, 1, HAL_MAX_DELAY);
    } else {
      goto sendnack;
    }
  } else {
    goto sendnack;
  }

  sendnack: data[0] = NACK;
  HAL_UART_Transmit(&huart2, (uint8_t *) data, 1, HAL_MAX_DELAY);
}

void CHECK_AND_SET_FLASH_PROTECTION(void) {
  FLASH_OBProgramInitTypeDef obConfig;

  /* Retrieves current OB */
  HAL_FLASHEx_OBGetConfig(&obConfig);

  /* If the first sector is not protected */
  if ((obConfig.WRPSector & OB_WRP_SECTOR_0) == OB_WRP_SECTOR_0) {
    HAL_FLASH_OB_Unlock(); //Unlocks OB
    obConfig.WRPState = OB_WRPSTATE_ENABLE; //Enables changing of WRP settings
    obConfig.WRPSector = OB_WRP_SECTOR_0; //Enables WP on first sector
    HAL_FLASHEx_OBProgram(&obConfig); //Programs the OB
    HAL_FLASH_OB_Launch(); //Ensures that the new configuration is saved in flash
    HAL_FLASH_OB_Lock(); //Locks OB
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
