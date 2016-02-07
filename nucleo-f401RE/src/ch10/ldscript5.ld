/* memory layout for an STM32F401RE */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 96K
}
 
 ENTRY(_start);

/* output sections */
SECTIONS
{
    /* Program code into FLASH */
    .text : ALIGN(4)
    {
        *(.isr_vector)    /* Vector table */
        *(.text .text*)          /* Program code */
        KEEP(*(.isr_vector))
    } >FLASH
 
    /* Initialized global and static variables (which 
       we don't have any in this example) into SRAM */

    /* Used by the startup to initialize data */
    _sidata = LOADADDR(.data);     
       
    .data : ALIGN(4)
    {
        _sdata = .;        /* create a global symbol at data start */
        
        *(.data)
        *(.data*)
        
        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end */        
    } >SRAM AT>FLASH
    
     /* Uninitialized data section */
		.bss (NOLOAD) : ALIGN(4)
		{
		  _sbss = .;         /* define a global symbol at bss start */
	  
		  *(.bss)
		  *(.bss*)
		  		
		  . = ALIGN(4);
		  _ebss = .;         /* define a global symbol at bss end */
		} >SRAM
	
		_end_static = _ebss;
		_Heap_Size = 0x190;
		_Heap_Limit = _end_static + _Heap_Size;
		_Min_Stack_Size = 0x400;		
		
	  /* User_heap_stack section, used to check that there is enough RAM left */
		._user_heap_stack :
		{
		  . = ALIGN(4);
		  . = . + _Heap_Size;
		  . = . + _Min_Stack_Size;
		  . = ALIGN(4);
		} >SRAM
		
	  /* Constant data goes into FLASH */
	  .rodata : ALIGN(4)
	  {
	    *(.rodata)         /* .rodata sections (constants) */
	    *(.rodata*)        /* .rodata* sections (strings, etc.) */
	  } >FLASH
	
}    