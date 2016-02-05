typedef unsigned long uint32_t;

/* memory and peripheral start addresses */
#define FLASH_BASE      0x08000000
#define SRAM_BASE       0x20000000
#define PERIPH_BASE     0x40000000

/* Work out end of RAM address as initial stack pointer */
#define SRAM_SIZE       96*1024     // STM32F401RE has 96 KB of RAM
#define SRAM_END        (SRAM_BASE + SRAM_SIZE)

/* RCC peripheral addresses applicable to GPIOA */
#define RCC_BASE        (PERIPH_BASE + 0x23800)
#define RCC_APB1ENR     ((uint32_t*)(RCC_BASE + 0x30))

/* GPIOA peripheral addresses */
#define GPIOA_BASE      (PERIPH_BASE + 0x20000)
#define GPIOA_MODER     ((uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       ((uint32_t*)(GPIOA_BASE + 0x14))

/* User functions */
void _start (void);
int main(void);
void delay(uint32_t count);

/* Minimal vector table */
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
    (uint32_t *)SRAM_END,   // initial stack pointer
    (uint32_t *)_start        // main as Reset_Handler
};

// Begin address for the initialisation values of the .data section.
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
__initialize_bss (uint32_t* region_begin, uint32_t* region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  uint32_t *p = region_begin;
  while (p < region_end)
    *p++ = 0;
}

void __attribute__ ((noreturn,weak))
_start (void)
{
	__initialize_data(&_sidata, &_sdata, &_edata);
	__initialize_bss(&_sbss, &_ebss);
	main();

	for(;;);
}

volatile uint32_t dataVar = 0x3f;
volatile uint32_t bssVar;

int main() {
    /* enable clock on GPIOA peripheral */
    *RCC_APB1ENR = 0x1;
    *GPIOA_MODER |= 0x400; // Sets MODER[11:10] = 0x1

    while(bssVar == 0) {
      *GPIOA_ODR = 0x20;
      delay(200000);
      *GPIOA_ODR = 0x0;
      delay(200000);
   	}
}

void delay(unsigned long count) {
    while(count--);
}
