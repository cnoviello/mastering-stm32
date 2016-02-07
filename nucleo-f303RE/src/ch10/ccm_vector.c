#include <stm32f3xx_hal.h>

extern const uint32_t _estack;

void SysTick_Handler(void);

uint32_t *ccm_vector_table[] __attribute__((section(".isr_vector_ccm"))) = {
    (uint32_t *)&_estack,   // initial stack pointer
    (uint32_t *) 0,        // Reset_Handler not relocatable
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) SysTick_Handler
};

void SysTick_Handler(void) {
  HAL_IncTick();
}
