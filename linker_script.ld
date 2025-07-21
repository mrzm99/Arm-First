/*--------------------------------------------------------------------------------------*/
/*!
 *      @file   linker_script.ld
 *      @date   2025.04.xx
 *      @author mrzm99
 *      @brief  linker script for STM32F405RG6
 *      @note
 */
/*--------------------------------------------------------------------------------------*/

/* Entry point */
ENTRY(reset_handlr)

/*--------------------------------------------------------------------------------------*/
/*! Memory definition
 */
MEMORY
{
    RAM     (rwx)   : ORIGIN = 0x20000000, LENGTH = 128K    /* 128KB RAM */
    FLASH   (rx)    : ORIGIN = 0x08000000, LENGTH = 1024K   /* 1MB Flash memory */
}

/*--------------------------------------------------------------------------------------*/
/*! Section definition
 */
SECTIONS
{
    /* program and read-only data */
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata)
        . = ALIGN(4);
        __etext = .;
    } > FLASH

    /* bss section */
    .bss :
    {
        . = ALIGN(4);
        __sbss = .;
        . = __sbss + 0x400;
        . = ALIGN(4);
        __estack = .;
        . = ALIGN(128);
        *(.vect_tbl_on_ram)
        . = ALIGN(4);
        *(.bss*)
        . = ALIGN(4);
        __ebss = .;
    } > RAM

    /* data section */
    .data :
    {
        . = ALIGN(4);
        __sdata = .;
        *(.data*)
        . = ALIGN(4);
        __edata = .;
    } > RAM AT > FLASH
    
    /* RAM end */
    __ram_end = 0x20000000 + 0x00020000;
}

