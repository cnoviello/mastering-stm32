/* memory layout for an STM32F401RE */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 96K
}
 
/* output sections */
SECTIONS
{
    /* program code into FLASH */
    .text : ALIGN(4)
    {
        *(.vector_table)    /* Vector table */
        *(.text)     /* Program code */
        KEEP(*(.vector_table))
    } >FLASH
 
    /* uninitialized global and static variables (which 
       we don't have any in this example) into SRAM */
    .data :
    {
        *(.data)            
    } >SRAM
}    