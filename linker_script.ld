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
    RAM     (rwx)   : ORIGIN = 0x20000000, LENGTH = 192K    /* 192KB RAM */
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

    /* data section */
    .data :
    {
        . = ALIGN(4);
        __sdata = .;
        *(.data*)
        . = ALIGN(4);
        __edata = .;
    } > RAM AT > FLASH

    /* bss section */
    .bss :
    {
        . = ALIGN(4);
        __sbss = .;
        __estack = __sbss + 0x400;
        *(.bss*)
        . = ALIGN(4);
        __ebss = .;
        _estack = __ebss; /* workaround for undefined symbol error */
    } > RAM
}

