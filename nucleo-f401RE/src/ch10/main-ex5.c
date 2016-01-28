#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
extern unsigned int _sidata;
// Begin address for the .data section; defined in linker script
extern unsigned int _sdata;
// End address for the .data section; defined in linker script
extern unsigned int _edata;
// Begin address for the .bss section; defined in linker script
extern unsigned int _sbss;
// End address for the .bss section; defined in linker script
extern unsigned int _ebss;

inline void
__attribute__((always_inline))
__initialize_data (unsigned int* from, unsigned int* region_begin, unsigned int* region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  unsigned int *p = region_begin;
  while (p < region_end)
    *p++ = *from++;
}

inline void
__attribute__((always_inline))
__initialize_bss (unsigned int* region_begin, unsigned int* region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  unsigned int *p = region_begin;
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

void __attribute__((weak))
_exit (int status __attribute__((unused)))
{
  errno = ENOSYS;

  for(;;);
}

int __attribute__((weak))
_getpid (int n __attribute__ ((unused)))
{
  return 1;
}

int __attribute__((weak))
_kill (int pid __attribute__((unused)), int sig __attribute__((unused)))
{
  errno = ENOSYS;
  return -1;
}

void *_sbrk(int incr) {
    extern uint32_t _end_static; /* Defined by the linker */
    extern unsigned int _Heap_Size;

    static uint32_t *heap_end;
    uint32_t *prev_heap_end;

    if (heap_end == 0) {
      heap_end = &_end_static;
    }
    prev_heap_end = heap_end;

    incr = (incr + 3) & (~3);
    if (heap_end + incr > &_end_static + _Heap_Size) {
      asm("BKPT");
      abort();
    }

    heap_end += incr;
    return (void*) prev_heap_end;
}

const char msg[] = "Hello World!";

int main() {
    /* enable clock on GPIOA and GPIOC peripherals */
    *RCC_APB1ENR = 0x1 | 0x4;
    *GPIOA_MODER |= 0x400; // Sets MODER[11:10] = 0x1

    int sch = sizeof(char);
    int sle = strlen(msg);

    char *heapMsg = (char*)malloc(sch*sle);
    memset(heapMsg, 0, strlen(msg));
    memcpy(heapMsg, msg, strlen(msg));

    while(strncmp(heapMsg, msg, strlen(msg)) == 0) {
      *GPIOA_ODR = 0x20;
      delay(200000);
      *GPIOA_ODR = 0x0;
      delay(200000);
   	}
}

void delay(unsigned long count) {
    while(count--);
}
