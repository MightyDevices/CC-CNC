/* Program Entry, set to mark it as "used" and avoid gcc warnings */
ENTRY(ResetHandler)

/* Memory Spaces Definitions */
MEMORY
{
	RAM      (RWX) : ORIGIN = 0x20000000, LENGTH = 32K
	FLASH    (RX)  : ORIGIN = 0x08000000, LENGTH = 32K
	EEPROM   (RW)  : ORIGIN = 0x08080000, LENGTH = 4K
}
/* code/data sections */

SECTIONS
{
    /* code */
    .text : 
    {
        /* exception & interrupt vectors */
        __flash_vectors = .;
        KEEP(*(.vectors))
        
        /* Program code */
        *(.text)                    
        *(.text.*)                  
        *(.stub .gnu.linkonce.t.*)
        *(.rodata .rodata.*)
        
        
        /* align code to 4 bytes */
        . = ALIGN(4); 
    } > FLASH
    
    /* needed for 64 bit division */
    __exidx_start = .;
    /* needed for 64 bit division */
	.ARM.exidx : 
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASH
    __exidx_end = .;
    
    /* all initialized data will go here */ 
    __datai_start = .;
    
    /* initialized data */
    .data : 
    {
        /* start of data */
        __data_start = .;
        
        /* data memory */             
        *(.data)                    
        *(.data.*)
        . = ALIGN(4);
        
        /* end of data */
        __data_end = .; 
    } > RAM AT > FLASH

    /* zeroed data */
    .bss :
    {
        /* bss section start */
        __bss_start = .;
        *(.bss)       
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        
        /* end of bss section */
        __bss_end = .;
    } > RAM
    
    /* stack pointer */
     __stack = ORIGIN(RAM) + LENGTH(RAM);
} 
