#include <stm32f3xx_hal.h>
#include <stm32f3xx_hal_gpio.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define HAL_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED

extern uint32_t _estack;
extern const uint32_t _sccm;

/* User functions */
void _start (void);
int main(void);

/* Minimal vector table */
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
    (uint32_t *)&_estack,   // initial stack pointer
    (uint32_t *)_start        // main as Reset_Handler
};

// Begin address for the initialization values of the .data section.
// defined in linker script
extern uint32_t _sidata;
// Begin address for the .data section; defined in linker script
extern uint32_t _sdata;
// End address for the .data section; defined in linker script
extern uint32_t _edata;
// Begin address for the initialization values of the .ccm section.
extern uint32_t _siccm;
// Begin address for the .ccm section; defined in linker script
extern uint32_t _sccmd;
// End address for the .ccm section; defined in linker script
extern uint32_t _eccmd;

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

void __attribute__ ((noreturn,weak))
_start (void) {
  SCB->VTOR = (uint32_t)&_sccm;  /* Relocate vector table to 0x1000 0000 */
  SYSCFG->RCR = 0xF;             /* Enable write protection for CCM memory */

	__initialize_data(&_sidata, &_sdata, &_edata);
	__initialize_data(&_siccm, &_sccmd, &_eccmd);
	__initialize_bss(&_sbss, &_ebss);
	main();

	for(;;);
}

uint32_t arr[20] __attribute__((section(".ccm")));

int main() {
    GPIO_InitTypeDef GPIO_InitStruct;

    HAL_Init();

    /* GPIO Ports Clock Enable */
    __GPIOA_CLK_ENABLE();

    /*Configure GPIO pin : LD2_Pin */
    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

    while(1) {
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
      HAL_Delay(500);
      arr[0] = 0xf;
    }
}

void delay(uint32_t count) {
    while(count--);
}
