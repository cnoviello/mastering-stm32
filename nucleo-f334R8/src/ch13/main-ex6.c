#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stm32f3xx_hal.h>

typedef unsigned long uint32_t;

/* Work out end of RAM address as initial stack pointer */
#define SRAM_SIZE       12*1024     // STM32F334R8 has 96 KB of RAM
#define SRAM_END        (SRAM_BASE + SRAM_SIZE)

/* RCC peripheral addresses applicable to GPIOA */
#define RCC_APB1ENR     ((uint32_t*)(RCC_BASE + (0x14)))

/* GPIOA peripheral addresses */
#define GPIOA_MODER     ((uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       ((uint32_t*)(GPIOA_BASE + 0x14))

extern uint32_t _estack;

/* User functions */
void _start (void);
int main(void);
void delay(uint32_t);

// Begin address for the initialization values of the .data section.
// defined in linker script
extern uint32_t _sidata;
// Begin address for the .data section; defined in linker script
extern uint32_t _sdata;
// End address for the .data section; defined in linker script
extern uint32_t _edata;
// Address in FLASH memory where .ccm section is stored; defined in linker script
extern uint32_t _slccm;
//// Begin address for the .ccm section; defined in linker script
extern uint32_t _sccm;
// End address for the .ccm section; defined in linker script
extern uint32_t _eccm;
// Size of the .ccm section; defined in linker script
extern uint32_t _ccmsize;
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
__initialize_bss (uint32_t* region_begin, uint32_t* region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  uint32_t *p = region_begin;
  while (p < region_end)
    *p++ = 0;
}

/* Minimal vector table */
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
    (uint32_t *)&_estack,   // initial stack pointer
    (uint32_t *)_start        // main as Reset_Handler
};

void __attribute__ ((noreturn,weak))
_start (void) {
  /* Copy the .ccm section from the FLASH memory (_slccm) into CCM memory  */
  memcpy(&_sccm, &_slccm, (size_t)&_ccmsize);

  SCB->VTOR = (uint32_t)&_sccm;  /* Relocate vector table to 0x1000 0000 */
  SYSCFG->RCR = 0xF;             /* Enable write protection for CCM memory */

	__initialize_data(&_sidata, &_sdata, &_edata);
	__initialize_bss(&_sbss, &_ebss);
	main();

	for(;;);
}

int main() {
  /* enable clock on GPIOA peripheral */
  *RCC_APB1ENR |= 0x1 << 17;
  *GPIOA_MODER |= 0x400; // Sets MODER[11:10] = 0x1

  SysTick_Config(4000000); //Underflows every 0.5s
}

void delay(uint32_t count) {
    while(count--);
}
