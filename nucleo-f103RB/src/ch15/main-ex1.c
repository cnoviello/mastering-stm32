typedef unsigned long uint32_t;

/* memory and peripheral start addresses */
#define FLASH_BASE      0x08000000
#define SRAM_BASE       0x20000000
#define PERIPH_BASE     0x40000000

/* Work out end of RAM address as initial stack pointer */
#define SRAM_SIZE       20*1024     // STM32F103RB has 20 KB of RAM
#define SRAM_END        (SRAM_BASE + SRAM_SIZE)

/* RCC peripheral addresses applicable to GPIOA */
#define RCC_BASE        (PERIPH_BASE + 0x21000)
#define RCC_APB2ENR      ((uint32_t*)(RCC_BASE + (0x18)))

/* GPIOA peripheral addresses */
#define GPIOA_BASE      (PERIPH_BASE + 0x10800)
#define GPIOA_CRL       ((uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       ((uint32_t*)(GPIOA_BASE + 0xC))

/* User functions */
int main(void);
void delay(uint32_t count);

/* Minimal vector table */
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
    (uint32_t *)SRAM_END,   // initial stack pointer
    (uint32_t *)main        // main as Reset_Handler
};

int main() {
    /* enable clock on GPIOA peripheral */
    *RCC_APB2ENR |= 0x1 | (0x1 << 2);
    *GPIOA_CRL = 0x44244444; // Sets MODER[27:26] = 0x1

    while(1) {
    	*GPIOA_ODR = 0x20;
      delay(100000);
    	*GPIOA_ODR = 0x0;
      delay(100000);
    }
}

void delay(uint32_t count) {
    while(count--);
}
