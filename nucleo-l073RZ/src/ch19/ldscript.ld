/* memory layout for an STM32L073RZ */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 192K
    SRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}
 
/* output sections */
SECTIONS
{
    /* Program code into FLASH */
    .text : ALIGN(4)
    {
        *(.isr_vector)    /* Vector table */
        *(.text)          /* Program code */
        KEEP(*(.isr_vector))
    } >FLASH
 
    /* Initialized global and static variables (which 
       we don't have any in this example) into SRAM */
    .data :
    {
        *(.data)           
    } >SRAM
}    