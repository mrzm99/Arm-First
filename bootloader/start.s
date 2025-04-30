/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   start.s
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief  
 *  @note
 */
 /*--------------------------------------------------------------------------------------*/

    .syntax unified
    .cpu cortex-m4
    .thumb

    .global main
    .global invalid_handlr
    .global reset_handlr

/*--------------------------------------------------------------------------------------*/
/*! @brief  vector table
 */
    .section .isr_vector
    .word   __estakc                @ Initial SP value
    .word   reset_handlr            @ Reset
    .word   invalid_handlr          @ NMI
    .word   invalid_handlr          @ Hard fault
    .word   invalid_handlr          @ Memory management fault
    .word   invalid_handlr          @ Bus faul
    .word   invalid_handlr          @ Usage fault
    .word   0                       @ Reserved
    .word   0                       @ Reserved
    .word   0                       @ Reserved
    .word   0                       @ Reserved
    .word   invalid_handlr          @ SVCall
    .word   0                       @ Reserved for Debug
    .word   0                       @ Reserved
    .word   invalid_handlr          @ PendSV
    .word   invalid_handlr          @ Systick

    .section .text
/*--------------------------------------------------------------------------------------*/
/*! @brief  reset handler
 */
    .type reset_handlr, %function
reset_handlr:
    ldr     r0, =__estakc   @ set stack pointer
    mov     sp, r0
    mov     r1, #0x55
    mov     r2, #0xAA
    mov     r3, #0x55
    mov     r4, #0xAA
    bl       main            @ never return

/*--------------------------------------------------------------------------------------*/
/*! @brief  default handler
 */
    .type invalid_handlr, %function
invalid_handlr:
    b       invalid_handlr          @ infinite loop